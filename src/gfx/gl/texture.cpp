/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "texture.h"
#include <cstring>

#include "../../common/debugging.h"
namespace redc { namespace gfx { namespace gl
{
  void GL_Texture::uninit() noexcept
  {
    if(tex_id) glDeleteTextures(1, &tex_id);
  }
  GL_Texture::~GL_Texture() noexcept
  {
    uninit();
  }
  void GL_Texture::allocate_(Vec<int> const& extents,
                             Texture_Format form,
                             Texture_Target target) noexcept
  {
    uninit();
    glGenTextures(1, &tex_id);

    // TODO: Figure something out, this is unexpected.
    glActiveTexture(GL_TEXTURE0);

    glBindTexture((GLenum) target, tex_id);

    // Remember the last three fields of glTexImage2D aren't significant in our
    // case because we have no data to copy over, we are just allocating room.

    if(target == Texture_Target::Tex_2D)
    {
      glTexImage2D((GLenum) target, 0, (GLenum) form, extents.x, extents.y,
                   0, GL_RGBA, GL_FLOAT, NULL);
    }
    else if(target == Texture_Target::Cube_Map)
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, (GLenum) form, extents.x,
                   extents.y, 0, GL_RGBA, GL_FLOAT, NULL);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, (GLenum) form, extents.x,
                   extents.y, 0, GL_RGBA, GL_FLOAT, NULL);

      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, (GLenum) form, extents.x,
                   extents.y, 0, GL_RGBA, GL_FLOAT, NULL);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, (GLenum) form, extents.x,
                   extents.y, 0, GL_RGBA, GL_FLOAT, NULL);

      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, (GLenum) form, extents.x,
                   extents.y, 0, GL_RGBA, GL_FLOAT, NULL);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, (GLenum) form, extents.x,
                   extents.y, 0, GL_RGBA, GL_FLOAT, NULL);
    }

    if(target == Texture_Target::Tex_2D)
    {
      glTexParameteri((GLenum) target, GL_TEXTURE_MIN_FILTER,
                      GL_NEAREST_MIPMAP_NEAREST);
      glTexParameteri((GLenum) target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      // These are actually set to their respective values by default anyway,
      // but this is more clear.
      glTexParameteri((GLenum) target, GL_TEXTURE_BASE_LEVEL, 0);
      glTexParameteri((GLenum) target, GL_TEXTURE_MAX_LEVEL, 5);

      glTexParameteri((GLenum) target, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri((GLenum) target, GL_TEXTURE_WRAP_R, GL_REPEAT);
    }
    else if(target == Texture_Target::Cube_Map)
    {
      glTexParameteri((GLenum) target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri((GLenum) target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glTexParameteri((GLenum) target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      glTexParameteri((GLenum) target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri((GLenum) target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // Save the format for later.
    format_ = form;
    target_ = target;
  }
  void GL_Texture::blit_tex2d_data(Volume<int> const& vol, Data_Type data_type,
                                   void const* in_data) noexcept
  {
    if(target_ != Texture_Target::Tex_2D)
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

    std::size_t type_size;

    switch(data_type)
    {
    case Data_Type::Float:
      type_size = sizeof(float);
      break;
    case Data_Type::UByte:
      type_size = sizeof(uint8_t);
      break;
    default:
      REDC_UNREACHABLE_MSG("Unsupported data type for texture data");
      return;
    }

    // Row size in bytes
    std::size_t row_size;
    GLenum data_format;

    switch(format_)
    {
    case Texture_Format::Rgba:
      row_size = type_size * 4 * vol.width;
      data_format = GL_RGBA;
      break;
    case Texture_Format::Rgb:
      row_size = type_size * 3 * vol.width;
      data_format = GL_RGB;
      break;
    case Texture_Format::Alpha:
      row_size = type_size * vol.width;
      data_format = GL_ALPHA;
      break;
    case Texture_Format::Depth:
      row_size = type_size * vol.width;
      data_format = GL_DEPTH_COMPONENT32F;
      break;
    case Texture_Format::Depth_Stencil:
      row_size = type_size * vol.width;
      data_format = GL_DEPTH32F_STENCIL8;
      break;
    case Texture_Format::Stencil:
      row_size = type_size * vol.width;
      data_format = GL_STENCIL_INDEX8;
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
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, vol.pos.x,
                    allocated_extents().y - vol.pos.y - vol.height,
                    vol.width, vol.height, data_format, (GLenum) data_type,
                    &out_data[0]);
    glGenerateMipmap(GL_TEXTURE_2D);

    delete[] out_data;
  }

  void GL_Texture::blit_cube_data(Cube_Map_Texture const& side,
                                  Volume<int> const& vol, Data_Type data_type,
                                  void const* in_data) noexcept
  {
    if(target_ != Texture_Target::Cube_Map)
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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);

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

    glTexSubImage2D(cubemap_side, 0, vol.pos.x,
                    allocated_extents().y - vol.pos.y - vol.height,
                    vol.width, vol.height, (GLenum) format_, (GLenum) data_type,
                    in_data);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  }

  void GL_Texture::bind(unsigned int loc) const noexcept
  {
    glActiveTexture(GL_TEXTURE0 + loc);
    glBindTexture((GLenum) target_, tex_id);
  }
} } }
