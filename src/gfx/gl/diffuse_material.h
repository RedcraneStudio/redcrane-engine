/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../imaterial.h"
#include "../program.h"
#include "../../color.h"
namespace survive
{
  namespace gfx
  {
    namespace gl
    {
      struct Diffuse_Material;
    }
  }

  struct gfx::gl::Diffuse_Material : public IMaterial
  {
    Diffuse_Material() noexcept;
    void use() const noexcept override;
  private:
    mutable Program prog_;

    bool diffuse_color_changed_;
    Color diffuse_color_;
  };
}
