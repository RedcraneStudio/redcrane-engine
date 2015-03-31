/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "factory.h"
#include "prep_mesh.h"
#include "prep_tex.h"
namespace survive
{
  namespace gfx
  {
    namespace gl
    {
      std::unique_ptr<Prepared_Mesh>
      Factory::prepare_mesh(Mesh&& mesh) noexcept
      {
        return std::make_unique<Prep_Mesh>(std::move(mesh));
      }
      std::unique_ptr<Prepared_Texture>
      Factory::prepare_texture(Texture&& tex) noexcept
      {
        return std::make_unique<Prep_Tex>(std::move(tex));
      }
    }
  }
}
