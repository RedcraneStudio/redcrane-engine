/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "render.h"
#include "../common/log.h"
#include "render/sdl_helper.h"
#include "../common/crash.h"
#include <uv.h>
namespace redc
{
  Render_Task::Render_Task(sail::Game const& game, po::variables_map const& vm,
                           std::string title, Vec<int> res, bool fullscreen,
                           bool vsync) noexcept : game_(&game)
  {
    if(!(window_ = init_sdl(title, res, fullscreen, vsync)))
    {
      crash();
    }
    SDL_SetRelativeMouseMode(SDL_TRUE);

    // Can we do this?
    uv_chdir("./assets");

    // Initialize a driver
    int w, h;
    SDL_GetWindowSize(window_, &w, &h);
    driver_ = std::make_unique<gfx::gl::Driver>(Vec<int>{w, h});

    mesh_cache_ = std::make_unique<gfx::Mesh_Cache>(*driver_);

    ocean_.init(*driver_, vm);
    envmap_.init(*driver_, vm);

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
  }
  Render_Task::~Render_Task() noexcept
  {
    uninit_sdl(window_);
  }
  void Render_Task::step() noexcept
  {
    // Handle events
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
      if(event.type == SDL_QUIT) should_close_ = true;
    }

    // Passing in the driver is redundant.
    ocean_.render(*driver_, cam_);
    envmap_.render(*driver_, cam_);

    SDL_GL_SwapWindow(window_);
  }

  bool Render_Task::should_close() noexcept { return should_close_; }
}
