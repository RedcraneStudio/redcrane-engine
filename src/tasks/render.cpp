/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "render.h"
#include "../common/log.h"
#include "render/sdl_helper.h"
#include "../common/crash.h"
namespace redc
{
  Render_Task::Render_Task(sail::Game const& game, std::string title,
                           Vec<int> res, bool fullscreen, bool vsync) noexcept
                           : game_(&game)
  {
    if(!init_sdl(title, res, fullscreen, vsync))
    {
      crash();
    }
    SDL_SetRelativeMouseMode(SDL_TRUE);
  }
  Render_Task::~Render_Task() noexcept
  {
    uninit_sdl();
  }
}
