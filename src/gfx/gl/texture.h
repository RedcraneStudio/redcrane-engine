/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "glad/glad.h"
#include "../itexture.h"
#include "driver.h"
namespace redc { namespace gfx { namespace gl
{
  struct GL_Texture : public ITexture
  {
    GL_Texture(Driver& driver);
    ~GL_Texture();

    void reinitialize() override;

    void blit_tex2d_data(Volume<std::size_t> const&, Texture_Format, Data_Type,
                         void const*) override;

    void blit_cube_data(Cube_Map_Texture const& side,
                        Volume<std::size_t> const& v,
                        Texture_Format, Data_Type, void const* data) override;

    void set_mag_filter(Texture_Filter filter) override;
    void set_min_filter(Texture_Filter filter) override;
    void set_wrap_s(Texture_Wrap wrap) override;
    void set_wrap_t(Texture_Wrap wrap) override;
    void set_wrap_r(Texture_Wrap wrap) override;
    void set_mipmap_level(unsigned int level) override;

    GLuint tex;
    GLenum gl_target;

    Texture_Format format;

    void bind(GLenum target);
  private:
    Driver* driver_;

    void allocate_tex_();
    void unallocate_tex_();

    virtual void allocate_(Vec<std::size_t> const&, Texture_Format,
                           Texture_Target type) override;

    void set_wrap_(GLenum coord,  Texture_Wrap wrap);
  };
} } }
