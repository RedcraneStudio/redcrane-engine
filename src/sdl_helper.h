/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include "common/vec.h"
#include "SDL.h"
#include "glad/glad.h"
namespace redc
{
  struct SDL_Init_Lock
  {
    SDL_Init_Lock() {}

    SDL_Init_Lock(SDL_Init_Lock&&);
    SDL_Init_Lock& operator=(SDL_Init_Lock&& lock);

    ~SDL_Init_Lock();

    SDL_Window* window;

    // I'm pretty sure this is actually a pointer. SDL treats it like one and
    // it can apparently be set to NULL so I guess we'll work with that.
    SDL_GLContext gl_context;
  };
  SDL_Init_Lock init_sdl(std::string title, Vec<int> res, bool fs, bool vsync);
}
