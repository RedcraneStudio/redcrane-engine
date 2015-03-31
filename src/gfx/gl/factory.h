/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../ifactory.h"
namespace survive
{
  namespace gfx
  {
    namespace gl
    {
      struct Factory;
    }
  }

  struct gfx::gl::Factory : public IFactory
  {
    std::unique_ptr<Prepared_Mesh> prepare_mesh(Mesh&& mesh) noexcept;
    std::unique_ptr<Prepared_Texture> prepare_texture(Texture&& tex) noexcept;
  };
}
