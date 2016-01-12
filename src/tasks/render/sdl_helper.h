/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include "../../common/vec.h"
#include "SDL.h"
#include "glad/glad.h"
namespace redc
{
  bool init_sdl(std::string title, Vec<int> res, bool fs, bool vsync) noexcept;
  void uninit_sdl() noexcept;
}
