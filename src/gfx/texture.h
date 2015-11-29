/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once

#include <vector>

#include "../common/vec.h"
#include "../common/volume.h"
#include "../common/color.h"
namespace game
{
  enum class Image_Format
  {
    Rgba, Depth
  };
  enum class Data_Type
  {
    Float, Integer
  };

  struct Texture
  {
    virtual ~Texture() noexcept {}

    inline void allocate(Vec<int> const& extents,
                         Image_Format form = Image_Format::Rgba) noexcept;

    // Assumes rgba format and normalized integer type
    inline void blit_data(Volume<int> const& vol, Color const* data) noexcept;
    // Assumes float type
    inline void blit_data(Volume<int> const& vol, float const*) noexcept;
    // No assumptions
    inline void blit_data(Volume<int> const& vol, Data_Type type,
                          void const* data) noexcept;

    inline Vec<int> allocated_extents() const noexcept;

    virtual inline void fill_data(Volume<int> const& v, Color const c) noexcept;

  private:
    virtual void allocate_(Vec<int> const&, Image_Format) noexcept = 0;

    virtual void blit_data_(Volume<int> const&,Color const* data) noexcept = 0;

    virtual void blit_data_(Volume<int> const&, float const*) noexcept = 0;

    virtual void blit_data_(Volume<int> const&, Data_Type,
                            void const*) noexcept = 0;

    Vec<int> extents_;
  };
  inline void Texture::allocate(Vec<int> const& extents,
                                Image_Format form) noexcept
  {
    extents_ = extents;
    allocate_(extents, form);
  }

  inline void Texture::blit_data(Volume<int> const& vol,
                                 Color const* data) noexcept
  {
    // We definitely don't want to pass this to the universal blit_data
    // function unless we know a sequence of Colors will indeed be contigous.
    // (As in struct members).

    blit_data_(vol, data);
  }
  inline void Texture::blit_data(Volume<int> const& vol,
                                 float const* data) noexcept
  {
    // We're not going to be clever and just pass this data into the universal
    // blit_data function so that the implementation can decide if it is
    // efficient to do that, etc.
    blit_data_(vol, data);
  }
  inline void Texture::blit_data(Volume<int> const& vol, Data_Type type,
                                 void const* data) noexcept
  {
    blit_data_(vol, type, data);
  }

  inline Vec<int> Texture::allocated_extents() const noexcept
  {
    return extents_;
  }

  inline void Texture::fill_data(Volume<int> const& v, Color const c) noexcept
  {
    std::vector<Color> colors(v.width * v.height, c);
    blit_data(v, &colors[0]);
  }
}
