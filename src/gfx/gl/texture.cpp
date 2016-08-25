/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "texture.h"
#include <cstring>

#include "common.h"

#include "../../common/debugging.h"
namespace redc { namespace gfx { namespace gl
{
  GL_Texture::GL_Texture(Driver& driver) : tex(0), driver_(&driver)
  {
    allocate_tex_();
  }
  GL_Texture::~GL_Texture()
  {
    unallocate_tex_();
  }
  void GL_Texture::reinitialize()
  {
    allocate_tex_();
    unallocate_tex_();
  }

  void GL_Texture::allocate_tex_()
  {
    glGenTextures(1, &tex);
  }
  void GL_Texture::unallocate_tex_()
  {
    glDeleteTextures(1, &tex);
  }

  void GL_Texture::bind(GLenum target)
  {
    // Bind to some target
    glBindTexture(target, tex);
  }

  void GL_Texture::set_mag_filter(Texture_Filter filter)
  {
    // Bind ourselves to the target we were allocated with.
    driver_->bind_texture(*this, this->gl_target);

    GLenum mag_filter = to_gl_texture_filter(filter);
    glTexParameteri(this->gl_target, GL_TEXTURE_MAG_FILTER, mag_filter);
  }
  void GL_Texture::set_min_filter(Texture_Filter filter)
  {
    driver_->bind_texture(*this, this->gl_target);

    GLenum min_filter = to_gl_texture_filter(filter);
    glTexParameteri(this->gl_target, GL_TEXTURE_MIN_FILTER, min_filter);
  }
  void GL_Texture::set_wrap_s(Texture_Wrap wrap)
  {
    set_wrap_(GL_TEXTURE_WRAP_S, wrap);
  }
  void GL_Texture::set_wrap_t(Texture_Wrap wrap)
  {
    set_wrap_(GL_TEXTURE_WRAP_T, wrap);
  }
  void GL_Texture::set_wrap_r(Texture_Wrap wrap)
  {
    set_wrap_(GL_TEXTURE_WRAP_R, wrap);
  }
  void GL_Texture::set_wrap_(GLenum coord, Texture_Wrap wrap)
  {
    driver_->bind_texture(*this, this->gl_target);
    GLenum glwrap = to_gl_texture_wrap(wrap);
    glTexParameteri(this->gl_target, coord, glwrap);
  }

  void GL_Texture::set_mipmap_level(unsigned int level)
  {
    driver_->bind_texture(*this, this->gl_target);
    glTexParameteri(this->gl_target, GL_TEXTURE_MAX_LEVEL, level);
  }

  void GL_Texture::allocate_(Vec<std::size_t> const& extents,
                             Texture_Format form,
                             Texture_Target target)
  {
    // Find the gl target now so we don't do it twice
    this->gl_target = to_gl_texture_target(target);
    driver_->bind_texture(*this, gl_target);

    // Save the format for later
    format = form;

    // Find the OpenGL internal format we should use.
    GLenum iformat = to_gl_texture_format(form);

    // Remember the last three fields of glTexImage2D aren't significant in our
    // case because we have no data to copy over, we are just allocating room.

    if(target == Texture_Target::Tex_2D)
    {
      glTexImage2D(gl_target, 0, iformat, extents.x, extents.y,
                   0, GL_RGBA, GL_FLOAT, NULL);
    }
    else if(target == Texture_Target::Cube_Map)
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, iformat, extents.x,
                   extents.y, 0, GL_RGBA, GL_FLOAT, NULL);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, iformat, extents.x,
                   extents.y, 0, GL_RGBA, GL_FLOAT, NULL);

      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, iformat, extents.x,
                   extents.y, 0, GL_RGBA, GL_FLOAT, NULL);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, iformat, extents.x,
                   extents.y, 0, GL_RGBA, GL_FLOAT, NULL);

      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, iformat, extents.x,
                   extents.y, 0, GL_RGBA, GL_FLOAT, NULL);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, iformat, extents.x,
                   extents.y, 0, GL_RGBA, GL_FLOAT, NULL);
    }
  }
  void GL_Texture::blit_tex2d_data(Volume<std::size_t> const& vol,
                                   Texture_Format data_format,
                                   Data_Type data_type,
                                   void const* in_data)
  {
    if(gl_target != GL_TEXTURE_2D)
    {
      log_d("Trying to blit 2D data to non-2d texture");
      return;
    }

    // Bail out if the volume isn't valid, we want to avoid the
    // GL_INVALID_OPERATION if we can.
    if(vol.width == 0 || vol.height == 0)
    {
      log_d("Ignoring zero volume blit attempt");
      return;
    }

    std::size_t type_size = data_type_size(data_type);
    std::size_t comps = texture_format_num_components(format);

    std::size_t row_size = type_size * comps * vol.width;

    // Allocate our data
    uint8_t* out_data = new uint8_t[row_size * vol.height];

    // Go backwards by row.
    for(std::size_t i = 0; i < vol.height; ++i)
    {
      // dest: output data starting from first row
      // src: input data starting from the bottom row
      // size: row_size
      std::memcpy(out_data + i * row_size,
                  (uint8_t*) in_data + row_size*vol.height - (i+1)*row_size,
                  row_size);
    }

    // We have a good array of data, where each successive row is going from
    // bottom to top, but we need to figure out our sub-region on the opengl
    // texture.

    GLenum gl_data_type = to_gl_data_type(data_type);
    GLenum gl_image_format = to_gl_texture_format(data_format);

    driver_->bind_texture(*this, gl_target);
    glTexSubImage2D(gl_target, 0, vol.pos.x,
                    allocated_extents().y - vol.pos.y - vol.height,
                    vol.width, vol.height, gl_image_format, gl_data_type,
                    &out_data[0]);
    glGenerateMipmap(gl_target);

    delete[] out_data;
  }

  void GL_Texture::blit_cube_data(Cube_Map_Texture const& side,
                                  Volume<std::size_t> const& vol,
                                  Texture_Format data_format,
                                  Data_Type data_type,
                                  void const* in_data)
  {
    if(gl_target != GL_TEXTURE_CUBE_MAP)
    {
      log_d("Trying to blit cube map to a texture that is not a cube map");
      return;
    }
    if(vol.width == 0 || vol.height == 0)
    {
      log_d("Ignoring zero volume blit attempt");
      return;
    }

    // No need to flip data for cube maps.

    GLenum cubemap_side;
    switch(side)
    {
      case Cube_Map_Texture::Positive_X:
        cubemap_side = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        break;
      case Cube_Map_Texture::Negative_X:
        cubemap_side = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
        break;
      case Cube_Map_Texture::Positive_Y:
        cubemap_side = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
        break;
      case Cube_Map_Texture::Negative_Y:
        cubemap_side = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
        break;
      case Cube_Map_Texture::Positive_Z:
        cubemap_side = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
        break;
      case Cube_Map_Texture::Negative_Z:
        cubemap_side = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
        break;
    }

    GLenum gl_data_type = to_gl_data_type(data_type);
    GLenum gl_image_format = to_gl_texture_format(data_format);

    driver_->bind_texture(*this, gl_target);
    glTexSubImage2D(cubemap_side, 0, vol.pos.x,
                    allocated_extents().y - vol.pos.y - vol.height, vol.width,
                    vol.height, gl_image_format, gl_data_type, in_data);
  }
} } }
