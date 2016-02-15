/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "render.h"
#include "../common/log.h"
#include "../common/plane.h"
#include "../common/crash.h"
#include "../common/gen_noise.h"
#include "render/camera.h"
#include <uv.h>
namespace redc
{
  Render_Task::Render_Task(sail::Game const& game, po::variables_map const& vm,
                           std::string title, Vec<int> res, bool fullscreen,
                           bool vsync) noexcept
                           : game_(&game),
                             sdl(title, res, fullscreen, vsync)
  {
    if(!sdl.window)
    {
      crash();
    }
    SDL_SetRelativeMouseMode(SDL_TRUE);

    // Can we do this?
    uv_chdir("../assets");

    // Initialize a driver
    int w, h;
    SDL_GetWindowSize(sdl.window, &w, &h);
    driver_ = std::make_unique<gfx::gl::Driver>(Vec<int>{w, h});

    mesh_cache_ = std::make_unique<gfx::Mesh_Cache>("obj", "cache");
    driver_->set_active_mesh_cache(mesh_cache_.get());

    envmap_.init(*driver_, vm);
    ocean_.init(*driver_, vm);
    boat_.init(*driver_, vm);

    // This is getting long. Make a bunch of functions that just establish
    // policy. Make_sail_camera, make_sail_effects, etc.

    cam_.projection_mode = gfx::Camera_Type::Perspective;
    cam_.perspective =
      gfx::Perspective_Cam_Params{glm::radians(90.0f),
                                  driver_->window_extents().x /
                                    (float) driver_->window_extents().y,
                                  .001f, 1000.0f};
    cam_.definition = gfx::Camera_Definition::Look_At;
    cam_.look_at.up = glm::vec3(0.0f, 1.0f, 0.0f);

    // The eye will be rotated around the boat.
    cam_.look_at.eye = glm::vec3(0.0f, 1.0f, -6.0f);
    cam_.look_at.look = glm::vec3(0.0f, 0.0f, 0.0f);

    boat_.motion.mass = 100;
    // We are using a radius of one for a solid sphere
    boat_.motion.angular.moment_of_inertia = 2.0f / 5.0f * 100.0f;
    // Approximate AABB of the boat, this will be used to calculate volume of
    // the boat.
    boat_aabb_ = aabb_from_min_max(glm::vec3(-0.4f, -0.12f, -1.8f),
                                   glm::vec3(+0.4f, +0.38f, +1.8f));
  }
  void Render_Task::step(float dt) noexcept
  {
    Vec<float> m_dif = {0.0f, 0.0f};
    // Handle events
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
      if(event.type == SDL_QUIT) should_close_ = true;

      if(event.type == SDL_MOUSEMOTION)
      {
        m_dif.x = event.motion.xrel / -500.0f;
        m_dif.y = event.motion.yrel / -500.0f;

        reposition_camera(cam_, m_dif);
      }
      if(event.type == SDL_KEYDOWN)
      {
        if(event.key.keysym.scancode == SDL_SCANCODE_W)
        {
          // Add a certain amount to eye in the direction of look - eye.
          auto eye_dir = glm::normalize(cam_.look_at.look - cam_.look_at.eye);
          eye_dir *= .1f;
          cam_.look_at.eye += eye_dir;
          cam_.look_at.look += eye_dir;
        }
      }
    }

    driver_->clear();

    // Passing in the driver is redundant.
    envmap_.render(*driver_, cam_);
    ocean_.render(*driver_, cam_);

    // Adjust the boat based on the current state of the ocean. Apply spring
    // forces based on the height of the water below the boat.

    // First try just the center.
    // Find the surface of the water.
    auto disp = boat_.motion.displacement.displacement;
    auto water_height =
      gen_noise(Vec<float>{disp.x, disp.z}, ocean_.get_ocean_gen_parameters());

    // Reset force
    collis::reset_force(boat_.motion);

    // mg
    auto mg = glm::vec3(0.0f, boat_.motion.mass * -9.81f, 0.0f);

    auto p_water = 1000.0f;

    Plane plane;

    // Rotate the normal with the boat's rotation
    plane.normal =
            glm::vec3(boat_.gen_model_mat() * glm::vec4(0.0f,1.0f,0.0f,0.0f));

    auto plane_pt = glm::vec3(boat_.gen_model_mat() *
                              glm::vec4(0.0f, boat_aabb_.min.y, 0.0f, 1.0f));

    std::array<glm::vec3, 4> boat_edges;

    // Front right
    boat_edges[0] = glm::vec3(boat_.gen_model_mat() *
                              glm::vec4(+boat_aabb_.min.x,
                                        +boat_aabb_.min.y,
                                        +boat_aabb_.min.z,
                                        1.0f));
    // Front left
    boat_edges[1] = glm::vec3(boat_.gen_model_mat() *
                              glm::vec4(-boat_aabb_.min.x,
                                        +boat_aabb_.min.y,
                                        +boat_aabb_.min.z,
                                        1.0f));
    // Back right
    boat_edges[2] = glm::vec3(boat_.gen_model_mat() *
                              glm::vec4(+boat_aabb_.min.x,
                                        +boat_aabb_.min.y,
                                        -boat_aabb_.min.z,
                                        1.0f));

    // Back left
    boat_edges[3] = glm::vec3(boat_.gen_model_mat() *
                              glm::vec4(-boat_aabb_.min.x,
                                        +boat_aabb_.min.y,
                                        -boat_aabb_.min.z,
                                        1.0f));

    plane.dist = -plane.normal.x * plane_pt.x +
                 -plane.normal.y * plane_pt.y +
                 -plane.normal.z * plane_pt.z;


    Vec<int> sample_dist{10, 20};

    // Intentionally not normalized
    auto to_front = boat_edges[1] - boat_edges[3];
    auto to_right = boat_edges[2] - boat_edges[3];

    auto num_values = 0;
    auto mean_distance = 0.0f;
    auto max_distance = 0.0f;
    auto min_distance = 0.0f;

    // Sample a set of points between the four edges, after projecting them
    // onto the water plane.
    for(int i = 0; i < area(sample_dist); ++i)
    {
      auto x = i % sample_dist.x; // 10 in the x
      auto y = i / sample_dist.x; // 20 in the y

      // We have our starting point.
      auto back_left = boat_edges[3];

      // Now, depending our position move forward and right
      // These are unit length, so scale them appropriately.
      // That means each increment of x and y is exactly that amount out of the
      // the total distance.
      // This will never make a vector exceeding its original length.
      back_left += to_right * (x / (float) sample_dist.x);
      back_left += to_front * (y / (float) sample_dist.y);

      // Now we are at some place.
      // TODO: Project onto the water plane and sample the height

      float sampled_height = fbm({back_left.x + ocean_.last_time_used_ / 5.0f,
                                  back_left.z + ocean_.last_time_used_ / 5.0f},
                                 ocean_.get_ocean_gen_parameters());

      //float sampled_height = glm::sin(ocean_.last_time_used_ +
                                      //back_left.z) * .4f;
      //float sampled_height = 0.0f;

      // We currently have a point on the boat
      // Find the signed distance from the boat hull to the water surface.

      // This gives us the signed distance such that a negative value means
      // the boat surface is above the water, if it is negative we don't care
      // about it.
      auto distance = std::max(0.0f, sampled_height - back_left.y);

      // Calculate force finally!
      auto F = distance * 10000;

      mean_distance += F;
      max_distance = std::max(max_distance, F);
      min_distance = std::min(min_distance, F);
      ++num_values;

      // World distance - world origin of the boat.
      collis::apply_force(boat_.motion,
                          glm::vec3(0.0f, F / (float) area(sample_dist), 0.0f),
                          back_left - boat_.motion.displacement.displacement);

      // Apply gravity
      collis::apply_force(boat_.motion, mg / (float)area(sample_dist),
                          back_left - boat_.motion.displacement.displacement);
    }

    // Solve motion
    collis::solve_motion(dt, boat_.motion);

    mean_distance /= num_values;

    log_i("distance: %", mean_distance);
    log_i("max dist: %", max_distance);
    log_i("min dist: %", min_distance);

    // Render boat.
    boat_.render(*driver_, cam_);

    SDL_GL_SwapWindow(sdl.window);
  }

  bool Render_Task::should_close() noexcept { return should_close_; }
}
