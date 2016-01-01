/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once

#include <vector>

#include "../common/vec.h"
#include "../common/volume.h"
#include "../common/color.h"
namespace redc
{
  enum class Image_Type
  {
    Tex_2D,
    Cube_Map
  };
  enum class Image_Format
  {
    Rgba, Depth
  };
  enum class Data_Type
  {
    Float, Unsigned_Byte
  };

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
                  Image_Format form = Image_Format::Rgba,
                  Image_Type type = Image_Type::Tex_2D) noexcept;

    virtual void blit_tex2d_data(Volume<int> const& vol, Data_Type type,
                                 void const* data) noexcept = 0;
    virtual void blit_cube_data(Cube_Map_Texture const& side,
                                Volume<int> const& vol, Data_Type type,
                                void const* data) noexcept = 0;

    inline Vec<int> allocated_extents() const noexcept { return extents_; }

  private:
    virtual void allocate_(Vec<int> const&, Image_Format,
                           Image_Type) noexcept = 0;

    Vec<int> extents_;
  };
}
