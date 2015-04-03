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
      Prep_Tex::Prep_Tex(Texture&& tex) noexcept
        : Prepared_Texture(std::move(tex))
      {
        glGenTextures(1, &tex_id);

        // Allow for different texture units.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex_id);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture().w, texture().h, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, texture().data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
      }

      void Prep_Tex::bind_(unsigned int loc) const noexcept
      {
        glActiveTexture(GL_TEXTURE0 + loc);
        glBindTexture(GL_TEXTURE_2D, tex_id);
      }
      void Prep_Tex::uninit_() noexcept
      {
        glDeleteTextures(1, &tex_id);
      }
    }
  }
}
