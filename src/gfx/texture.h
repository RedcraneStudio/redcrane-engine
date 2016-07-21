/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once

#include <vector>

#include "../common/vec.h"
#include "../common/volume.h"
#include "../common/color.h"

#include "enums.h"
namespace redc
{
  enum class Cube_Map_Texture
  {
    Positive_X,
    Negative_X,
    Positive_Y,
    Negative_Y,
    Positive_Z,
    Negative_Z,
  };

  struct Texture
  {
    virtual ~Texture() noexcept {}

    void allocate(Vec<int> const& extents,
                  Texture_Format form = Texture_Format::Rgba,
                  Texture_Target type = Texture_Target::Tex_2D) noexcept;

    virtual void blit_tex2d_data(Volume<int> const& vol, Data_Type type,
                                 void const* data) noexcept = 0;
    virtual void blit_cube_data(Cube_Map_Texture const& side,
                                Volume<int> const& vol, Data_Type type,
                                void const* data) noexcept = 0;

    inline Vec<int> allocated_extents() const noexcept { return extents_; }

  private:
    virtual void allocate_(Vec<int> const&, Texture_Format,
                           Texture_Target) noexcept = 0;

    Vec<int> extents_;
  };
}
