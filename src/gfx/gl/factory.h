/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include "../prepared_mesh.h"
#include "../prepared_texture.h"
namespace survive
{
  namespace gfx
  {
    namespace gl
    {
      struct Factory;
    }
  }

  struct gfx::gl::Factory
  {
    std::unique_ptr<Prepared_Mesh> prepare_mesh(Mesh&& mesh) noexcept;
    std::unique_ptr<Prepared_Texture> prepare_texture(Texture&& tex) noexcept;
  };
}
