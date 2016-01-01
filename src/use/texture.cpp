/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "texture.h"
#include "../gfx/support/texture_load.h"
namespace game { namespace gfx
{
  std::unique_ptr<Texture> load_texture(gfx::IDriver& d,
                                        std::string filename) noexcept
  {
    auto tex = d.make_texture_repr();

    auto data = load_png_data(filename);
    blit_image(*tex, data);

    return tex;
  }
  std::unique_ptr<Texture> load_cubemap(gfx::IDriver& d, std::string front,
                                        std::string back, std::string right,
                                        std::string left, std::string top,
                                        std::string bottom) noexcept
  {
    auto tex = d.make_texture_repr();

    auto front_data = load_png_data(front);
    auto back_data = load_png_data(back);
    auto right_data = load_png_data(right);
    auto left_data = load_png_data(left);
    auto top_data = load_png_data(top);
    auto bottom_data = load_png_data(bottom);

    allocate_cube_map(*tex, front_data);

    blit_cube_map_face(*tex, Cube_Map_Texture::Positive_X, right_data);
    blit_cube_map_face(*tex, Cube_Map_Texture::Negative_X, left_data);

    blit_cube_map_face(*tex, Cube_Map_Texture::Positive_Y, top_data);
    blit_cube_map_face(*tex, Cube_Map_Texture::Negative_Y, bottom_data);

    blit_cube_map_face(*tex, Cube_Map_Texture::Positive_Z, back_data);
    blit_cube_map_face(*tex, Cube_Map_Texture::Negative_Z, front_data);

    return tex;
  }
} }
