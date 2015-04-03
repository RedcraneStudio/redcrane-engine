/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include "../common/color.h"
#include "prepared_mesh.h"
#include "prepared_texture.h"
namespace strat
{
  namespace gfx
  {
    struct IDriver
    {
      virtual ~IDriver() noexcept {}

      virtual std::unique_ptr<Prepared_Mesh>
        prepare_mesh(Mesh&& mesh) noexcept = 0;
      virtual std::unique_ptr<Prepared_Texture>
        prepare_texture(Texture&& tex) noexcept = 0;

      virtual void clear_color_value(Color const& color) noexcept = 0;
      virtual void clear_depth_value(float val) noexcept = 0;

      virtual void clear() noexcept = 0;
      virtual void clear_color() noexcept = 0;
      virtual void clear_depth() noexcept = 0;
    };
  }
}
