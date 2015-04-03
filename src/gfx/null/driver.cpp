/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "driver.h"
namespace strat
{
  namespace gfx
  {
    namespace null
    {
      struct Prep_Mesh : public Prepared_Mesh
      {
        Prep_Mesh(Mesh&& mesh) noexcept : Prepared_Mesh(std::move(mesh)) {}
        inline ~Prep_Mesh() noexcept { uninit(); }
        inline void render_() const noexcept override {}
        inline void uninit_() noexcept override {}
      };

      struct Prep_Tex : public Prepared_Texture
      {
        Prep_Tex(Texture&& tex) noexcept : Prepared_Texture(std::move(tex)) {}
        inline ~Prep_Tex() noexcept { uninit(); }
        inline void bind_(unsigned int) const noexcept override {}
        inline void uninit_() noexcept override {}
      };

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
    }
  }
}
