/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "render.h"
#include "../common/log.h"
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

    mesh_cache_ = std::make_unique<gfx::Mesh_Cache>(*driver_);
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
    boat_.motion.angular.moment_of_inertia = 100 * 2 / 5;
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
    collis::apply_force(boat_.motion,
                        glm::vec3(0.0f, boat_.motion.mass * -9.81f, 0.0f));


    auto p_water = 1000.0f;

    // Use the aabb and do a bunch of segments
    // _________
    // |_|_|_|_|
    // |_|_|_|_|
    // |_|_|_|_|
    // |_|_|_|_|
    // |_|_|_|_|
    // |_|_|_|_|
    // |_|_|_|_|
    // |_|_|_|_|
    // |_|_|_|_|
    // |_|_|_|_|
    // |_|_|_|_|

    // ^^ That's our boat, in reality it is a ton smoother. For now we consider
    // rows to be left to right and colums top to bottom
    auto cell_width = boat_aabb_.width / 4.0f;
    auto cell_height = boat_aabb_.depth / 10.0f;

    // From top to bottom
    for(int segment_y_i = 0; segment_y_i < 10; ++segment_y_i)
    {
      // Left to right
      for(int segment_x_i = 0; segment_x_i < 4; ++segment_x_i)
      {
        // Where are we on the x axis, in boat model space?

        Vec<float> boat_grid_pos {
          0.0f + cell_width / 2 + cell_width * segment_x_i,
          0.0f + cell_height / 2 + cell_height * segment_y_i
        };

        // Okay now we have that position, put it in world space
        glm::vec3 world_pt = glm::vec3(boat_grid_pos.x, 0.0f, boat_grid_pos.y);

        // We do care about translation so use 1.0f as w.
        world_pt = glm::vec3(boat_.gen_model_mat() * glm::vec4(world_pt, 1.0f));

        // Now find that position and sample the height of the water at that
        // point.
        auto water_height = gen_noise(Vec<float>{world_pt.x, world_pt.z},
                                      ocean_.get_ocean_gen_parameters());
        water_height = 0.0f;

        // Now calculate the volume below this grid cell that is currently
        // displacing water.

        // The volume is the small grid where the height is adjusted by the
        // water surface.

        // Start at the boat center
        // Go down to the bottom of the boat
        // The distance between here and the water surface, where the water
        // surface is above us is the volume height
        auto volume_height = water_height -
                             (boat_.motion.displacement.displacement.y +
                              boat_aabb_.min.y);

        auto volume = cell_width * volume_height *
                      cell_height; // Cell height is more like length

        // B = pVg
        auto B_force = glm::vec3(0.0f, p_water * volume * 9.81f, 0.0f);
        // Use the original coordinates to apply the torque
        auto torque_r = glm::vec3(boat_grid_pos.x, 0.0f, boat_grid_pos.y);
        // Half the center of the boat, ez pz with the aabb
        torque_r -= glm::vec3(boat_aabb_.width / 2.0f, 0.0f,
                              boat_aabb_.depth / 2.0f);

        // Only apply the force if we are the only one
        if(segment_x_i == 0 && segment_y_i == 0)
        {
          collis::apply_force(boat_.motion, B_force, torque_r);
        }
      }
    }

    // Solve motion
    collis::solve_motion(dt, boat_.motion);

    // Render boat.
    boat_.render(*driver_, cam_);

    SDL_GL_SwapWindow(sdl.window);
  }

  bool Render_Task::should_close() noexcept { return should_close_; }
}
