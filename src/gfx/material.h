/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include "../common/color.h"
#include "prepared_texture.h"
#include "idriver.h"
namespace strat
{
  namespace gfx
  {
    struct Material
    {
      Color diffuse_color;
      std::shared_ptr<Prepared_Texture> texture;
    };

    Material load_material(gfx::IDriver&, std::string const&) noexcept;
  }
}
