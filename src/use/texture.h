/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <string>
#include "../gfx/idriver.h"
#include "../gfx/texture.h"
namespace game { namespace gfx
{
  std::unique_ptr<Texture> load_texture(gfx::IDriver& d,
                                        std::string filename) noexcept;

  std::unique_ptr<Texture> load_cubemap(gfx::IDriver& d,
                                        std::string front, std::string back,
                                        std::string right, std::string left,
                                        std::string top,
                                        std::string bottom) noexcept;
} }
