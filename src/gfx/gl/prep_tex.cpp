/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "prep_tex.h"
namespace strat
{
  namespace gfx
  {
    namespace gl
    {
      Prep_Tex::Prep_Tex(Texture& tex) noexcept
      {
        glGenTextures(1, &tex_id);

        // Allow for different texture units.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex_id);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.w, tex.h, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, tex.data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
      }
      Prep_Tex::~Prep_Tex() noexcept
      {
        glDeleteTextures(1, &tex_id);
      }

      Prep_Tex::Prep_Tex(Prep_Tex&& t) noexcept : tex_id(t.tex_id)
      {
        t.tex_id = 0;
      }
      Prep_Tex& Prep_Tex::operator=(Prep_Tex&& t) noexcept
      {
        tex_id = t.tex_id;
        t.tex_id = 0;

        return *this;
      }

      // Binds *this* texture to a given texture unit spot.
      void Prep_Tex::bind(unsigned int loc) const noexcept
      {
        glActiveTexture(GL_TEXTURE0 + loc);
        glBindTexture(GL_TEXTURE_2D, tex_id);
      }
    }
  }
}
