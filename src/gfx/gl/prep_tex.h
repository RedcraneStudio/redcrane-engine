/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "glad/glad.h"
#include "../texture.h"
namespace game
{
  namespace gfx
  {
    namespace gl
    {
      struct Prep_Tex
      {
        Prep_Tex(Texture& tex) noexcept;
        ~Prep_Tex() noexcept;

        Prep_Tex(Prep_Tex&&) noexcept;
        Prep_Tex& operator=(Prep_Tex&&) noexcept;

        GLuint tex_id;

        void bind(unsigned int loc) const noexcept;
      };
    }
  }
}
