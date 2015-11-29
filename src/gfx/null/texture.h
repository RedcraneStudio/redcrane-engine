/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../texture.h"
namespace game { namespace gfx { namespace null
{
  class Null_Texture : public Texture
  {
    void allocate_(Vec<int> const&, Image_Format) noexcept override {}

    void blit_data_(Volume<int> const&, Color const*) noexcept override {}

    void blit_data_(Volume<int> const&, float const*) noexcept override {}

    void blit_data_(Volume<int> const&, Data_Type,
                    void const*) noexcept override {}
  };
} } }
