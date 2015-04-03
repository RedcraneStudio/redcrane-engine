/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../idriver.h"
namespace survive
{
  namespace gfx
  {
    namespace gl
    {
      struct Driver;
    }
  }

  struct gfx::gl::Driver : public IDriver
  {
    std::unique_ptr<Prepared_Mesh> prepare_mesh(Mesh&& mesh) noexcept override;
    std::unique_ptr<Prepared_Texture>
    prepare_texture(Texture&& tex) noexcept override;

    void clear_color_value(Color const&) noexcept override;
    void clear_depth_value(float) noexcept override;

    void clear() noexcept override;
    void clear_color() noexcept override;
    void clear_depth() noexcept override;
  };
}
