/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "texture.h"
#include <cstring>
namespace game { namespace gfx { namespace gl
{
  void GL_Texture::allocate_(Vec<int> const& extents,
                             Image_Format form) noexcept
  {
    glGenTextures(1, &tex_id);

    // TODO: Figure something out, this is unexpected.
    glActiveTexture(GL_TEXTURE0);

    texture_type = GL_TEXTURE_2D;
    //if(extents.x != extents.y) texture_type = GL_TEXTURE_RECTANGLE;

    glBindTexture(texture_type, tex_id);

    if(form == Image_Format::Rgba)
    {
      glTexImage2D(texture_type, 0, GL_RGBA, extents.x, extents.y, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, NULL);
    }
    else if(form == Image_Format::Depth)
    {
      glTexImage2D(texture_type, 0, GL_DEPTH_COMPONENT, extents.x, extents.y,
                   0, GL_RGBA, GL_FLOAT, NULL);
    }
    switch(form)
    {
      case Image_Format::Rgba:
        // We don't care about the format, type, and data yet.
        glTexImage2D(texture_type, 0, GL_RGBA, extents.x, extents.y, 0,
                     GL_RGBA, GL_FLOAT, NULL);
        break;
      case Image_Format::Depth:
        glTexImage2D(texture_type, 0, GL_DEPTH_COMPONENT, extents.x, extents.y,
                     0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        break;
    }

    glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER,
                    GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // These are actually set to their respective values by default anyway, but
    // this is more clear.
    glTexParameteri(texture_type, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(texture_type, GL_TEXTURE_MAX_LEVEL, 5);

    glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(texture_type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    // Save the format for later.
    format_ = form;
  }
  void GL_Texture::blit_data_(Volume<int> const& vol, Data_Type type,
                              void const* in_data) noexcept
  {
    // Bail out if the volume isn't valid, we want to avoid the
    // GL_INVALID_OPERATION if we can.
    if(vol.width == 0 || vol.height == 0) return;

    std::size_t type_size;
    GLenum data_type;

    switch(type)
    {
      case Data_Type::Float:
        type_size = sizeof(float);
        data_type = GL_FLOAT;
        break;
      case Data_Type::Integer:
        type_size = sizeof(uint8_t);
        data_type = GL_UNSIGNED_BYTE;
        break;
    }

    // Row size in bytes
    std::size_t row_size;
    GLenum data_format;

    switch(format_)
    {
      case Image_Format::Rgba:
        row_size = type_size * 4 * vol.width;
        data_format = GL_RGBA;
        break;
      case Image_Format::Depth:
        row_size = type_size * vol.width;
        data_format = GL_DEPTH_COMPONENT;
        break;
    }

    // Allocate our data
    uint8_t* out_data = new uint8_t[row_size * vol.height];

    // Go backwards by row.
    for(int i = 0; i < vol.height; ++i)
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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(texture_type, tex_id);
    glTexSubImage2D(texture_type, 0, vol.pos.x,
                    allocated_extents().y - vol.pos.y - vol.height,
                    vol.width, vol.height, data_format, data_type, &out_data[0]);
    glGenerateMipmap(texture_type);

    delete[] out_data;
  }

  void GL_Texture::bind(unsigned int loc) const noexcept
  {
    glActiveTexture(GL_TEXTURE0 + loc);
    glBindTexture(texture_type, tex_id);
  }
} } }
