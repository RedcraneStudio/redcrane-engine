/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "driver.h"
#include "prep_mesh.h"
#include "prep_tex.h"
namespace survive
{
  namespace gfx
  {
    namespace gl
    {
      std::unique_ptr<Prepared_Mesh>
      Driver::prepare_mesh(Mesh&& mesh) noexcept
      {
        return std::make_unique<Prep_Mesh>(std::move(mesh));
      }
      std::unique_ptr<Prepared_Texture>
      Driver::prepare_texture(Texture&& tex) noexcept
      {
        return std::make_unique<Prep_Tex>(std::move(tex));
      }

      void Driver::clear() noexcept
      {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      }
      void Driver::clear_color() noexcept
      {
        glClear(GL_COLOR_BUFFER_BIT);
      }
      void Driver::clear_depth() noexcept
      {
        glClear(GL_DEPTH_BUFFER_BIT);
      }
    }
  }
}
