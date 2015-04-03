/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../prepared_texture.h"
#include "glad/glad.h"
namespace survive
{
  namespace gfx
  {
    namespace gl
    {
      struct Prep_Tex : public Prepared_Texture
      {
        Prep_Tex(Texture&&) noexcept;
        // It's required by the base class that we do this.
        ~Prep_Tex() noexcept { uninit(); }

        GLuint tex_id;
      private:
        void bind_(unsigned int loc) const noexcept override;
        void uninit_() noexcept override;
      };
    }
  }
}
