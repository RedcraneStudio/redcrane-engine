/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include <memory>
#include "../common/color.h"
#include "../common/maybe_owned.hpp"
#include "texture.h"
namespace game
{
  namespace gfx
  {
    struct Material
    {
      Color diffuse_color;
      Maybe_Owned<Texture> texture;
    };

    Material load_material(std::string const&) noexcept;

    struct IDriver;
    void prepare_material(IDriver& driver, Material const& mat) noexcept;
    void remove_material(IDriver& driver, Material const& mat) noexcept;
    void bind_material(IDriver& driver, Material const& mat) noexcept;
  }
}
