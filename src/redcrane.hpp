/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 *
 * The engine's C interface for the engine to use
 */
#include <cstdint>

#include "gfx/idriver.h"
#include "SDL.h"

namespace redc
{
  struct Engine
  {
    gfx::IDriver& driver;
    SDL_Window* window;
  };
}
