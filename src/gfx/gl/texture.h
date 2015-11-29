/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "glad/glad.h"
#include "../texture.h"
namespace game { namespace gfx { namespace gl
{
  struct GL_Texture : public Texture
  {
    void allocate_(Vec<int> const&, Image_Format) noexcept override;

    inline void blit_data_(Volume<int> const& vol,
                           Color const* data) noexcept override
    {
      // Fuck it, it might work.
      blit_data_(vol, Data_Type::Integer, data);
    }

    inline void blit_data_(Volume<int> const& vol,
                           float const* data) noexcept override
    {
      blit_data_(vol, Data_Type::Float, data);
    }

    void blit_data_(Volume<int> const&, Data_Type,
                    void const*) noexcept override;

    GLuint tex_id;
    GLenum texture_type;

    Image_Format format_;

    void bind(unsigned int loc) const noexcept;
  };
} } }
