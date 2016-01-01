/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../texture.h"
namespace redc { namespace gfx { namespace null
{
  class Null_Texture : public Texture
  {
    void allocate_(Vec<int> const&, Image_Format, Image_Type) noexcept override {}

    void blit_tex2d_data(Volume<int> const&, Data_Type,
                         void const*) noexcept override {}

    void blit_cube_data(Cube_Map_Texture const& side, Volume<int> const& vol,
                        Data_Type type, void const* data) noexcept override {}
  };
} } }
