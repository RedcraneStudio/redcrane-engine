/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "glad/glad.h"
#include "../texture.h"
namespace redc { namespace gfx { namespace gl
{
  struct GL_Texture : public Texture
  {
  private:
    void uninit() noexcept;

    virtual void allocate_(Vec<int> const&, Image_Format,
                           Image_Type type) noexcept override;

  public:
    ~GL_Texture() noexcept;
    void blit_tex2d_data(Volume<int> const&, Data_Type,
                         void const*) noexcept override;

    void blit_cube_data(Cube_Map_Texture const& side, Volume<int> const& v,
                        Data_Type, void const* data) noexcept override;

    GLuint tex_id;
    GLenum texture_type;

    Image_Format format_;
    Image_Type type_;

    void bind(unsigned int loc) const noexcept;
  };
} } }
