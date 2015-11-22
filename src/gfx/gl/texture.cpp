/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "texture.h"
namespace game { namespace gfx { namespace gl
{
  void GL_Texture::allocate_(Vec<int> const& extents,
                             Texture_Format form) noexcept
  {
    glGenTextures(1, &tex_id);

    glActiveTexture(GL_TEXTURE0);

    texture_type = GL_TEXTURE_2D;
    //if(extents.x != extents.y) texture_type = GL_TEXTURE_RECTANGLE;

    glBindTexture(texture_type, tex_id);

    if(form == Texture_Format::Rgba)
    {
      glTexImage2D(texture_type, 0, GL_RGBA, extents.x, extents.y, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, NULL);
    }
    else if(form == Texture_Format::Grayscale)
    {
      glTexImage2D(texture_type, 0, GL_RGBA, extents.x, extents.y, 0, GL_RED,
                   GL_FLOAT, NULL);
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
  }
  void GL_Texture::blit_data_(Volume<int> const& vol,
                  Color const* colors) noexcept
  {
    // Bail out if the volume isn't valid, we want to avoid the
    // GL_INVALID_OPERATION if we can.
    if(vol.width == 0 || vol.height == 0) return;

    std::vector<unsigned char> data;
    data.reserve(vol.width * vol.height * 4);

    // Go from the end to the beginning adding each byte to the end of the
    // vector.
    for(int i = 0; i < vol.width * vol.height * 4; ++i)
    {
      Color::c_t value;

      // Basically find our offset into the array so far.
      int pos = i / 4;
      // Use that offset to determine our coordinate (relative to subtexture
      // origin).
      int x = pos % vol.width;
      int y = pos / vol.width;

      // Use the row from the bottom first, relative to the given sub-volume.
      y = vol.height - y - 1;

      // Now recalculate our position **in our colors array** to get that
      // coordinate calculated above.
      pos = y * vol.width + x;

      // Our component just keeps repeating 0-3 and again, it's independent of
      // the coordinate.
      int component = i % 4;
      switch(component)
      {
        case 0:
          value = colors[pos].r;
          break;
        case 1:
          value = colors[pos].g;
          break;
        case 2:
          value = colors[pos].b;
          break;
        case 3:
          value = colors[pos].a;
          break;
        default:
          value = 0x00;
      }
      data.push_back(value);
    }

    // We have a good array of data, where each successive row is going from
    // bottom to top, but we need to figure out our sub-region on the opengl
    // texture.

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(texture_type, tex_id);
    glTexSubImage2D(texture_type, 0, vol.pos.x,
                    allocated_extents().y - vol.pos.y - vol.height,
                    vol.width, vol.height, GL_RGBA, GL_UNSIGNED_BYTE,
                    &data[0]);
    glGenerateMipmap(texture_type);
  }

  inline void GL_Texture::blit_data_(Volume<int> const& vol,
                                     float const* gray_data) noexcept
  {
    // Bail out if the volume isn't valid, we want to avoid the
    // GL_INVALID_OPERATION if we can.
    if(vol.width == 0 || vol.height == 0) return;

    std::vector<float> data;
    data.reserve(vol.width * vol.height);

    // Go from the end to the beginning adding each byte to the end of the
    // vector.
    for(int i = 0; i < vol.width * vol.height; ++i)
    {
      // Use that offset to determine our coordinate (relative to subtexture
      // origin).
      int x = i % vol.width;
      int y = i / vol.width;

      // Use the row from the bottom first, relative to the given sub-volume.
      y = vol.height - y - 1;

      // Now recalculate our position **in our colors array** to get that
      // coordinate calculated above.
      auto pos = y * vol.width + x;

      data.push_back(gray_data[pos]);
    }

    // We have a good array of data, where each successive row is going from
    // bottom to top, but we need to figure out our sub-region on the opengl
    // texture.

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(texture_type, tex_id);
    glTexSubImage2D(texture_type, 0, vol.pos.x,
                    allocated_extents().y - vol.pos.y - vol.height,
                    vol.width, vol.height, GL_RED, GL_FLOAT,
                    &data[0]);
    glGenerateMipmap(texture_type);
  }

  void GL_Texture::bind(unsigned int loc) const noexcept
  {
    glActiveTexture(GL_TEXTURE0 + loc);
    glBindTexture(texture_type, tex_id);
  }
} } }
