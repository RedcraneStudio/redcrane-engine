/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include <memory>
#include "../common/color.h"
#include "../common/maybe_owned.hpp"
#include "../common/software_texture.h"
#include "shader.h"
namespace game
{
  namespace gfx
  {
    struct Material
    {
      Material() noexcept;

      Color diffuse_color;
      Maybe_Owned<Software_Texture> texture;
    };

    Material load_material(std::string const&) noexcept;

    struct IDriver;
    void prepare_material(IDriver& driver, Material& mat) noexcept;
    void bind_material(IDriver&, Material const&) noexcept;
  }
}
