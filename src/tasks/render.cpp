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
    uv_chdir("./assets");

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
    cam_.look_at.eye = glm::vec3(0.0f, 5.0f, -6.0f);
    cam_.look_at.look = glm::vec3(0.0f, 0.0f, 0.0f);

    boat_.motion.mass = 100;
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

    // Now use the boats height and apply a spring force with the distance
    // being equal to height the boat should be in the water, this is normally
    // determined by force of buoyancy but we are going to fake it for now.
    // F = kx

    // If we say up is positive (not sure) we expect the displacement to be the
    // bigger value so it must come first
    auto spring_k = 100.0f;
    auto force = glm::vec3(0.0f, (disp.y - water_height) * -spring_k, 0.0f);

    auto damp_factor = 5.0f;
    auto dampening = boat_.motion.displacement.velocity.y * -damp_factor;

    collis::reset_force(boat_.motion);

    collis::apply_force(boat_.motion, force);
    collis::apply_force(boat_.motion, glm::vec3(0.0f, dampening, 0.0f));

    collis::solve_motion(dt, boat_.motion);

    // Render boat.
    boat_.render(*driver_, cam_);

    SDL_GL_SwapWindow(sdl.window);
  }

  bool Render_Task::should_close() noexcept { return should_close_; }
}
