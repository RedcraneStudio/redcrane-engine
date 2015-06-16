/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include "../texture.h"
namespace game
{
  void load_png(std::string png, Texture& t, bool alloc_once = false) noexcept;
}
