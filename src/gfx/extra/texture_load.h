/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include "../itexture.h"
namespace redc { namespace gfx
{
  struct Image
  {
    std::vector<Color> data;
    Vec<std::size_t> extents;
  };
  Image load_png_data(std::string filename);
  void blit_image(ITexture& t, Image const& img);

  void allocate_cube_map(ITexture& t, Vec<std::size_t> const& img);
  void blit_cube_map_face(ITexture& t, Cube_Map_Texture const& side,
                          Image const& img);
} }
