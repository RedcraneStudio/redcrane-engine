/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include "../texture.h"
namespace game
{
  struct Image
  {
    std::vector<Color> data;
    Vec<int> extents;
  };
  Image load_png_data(std::string filename) noexcept;
  void blit_image(Texture& t, Image const& img) noexcept;

  void allocate_cube_map(Texture& t, Image const& img) noexcept;
  void blit_cube_map_face(Texture& t, Cube_Map_Texture const& side,
                          Image const& img) noexcept;
}
