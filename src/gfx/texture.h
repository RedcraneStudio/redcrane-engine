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
  enum class Texture_Format
  {
    Rgba, Grayscale
  };

  struct Texture
  {
    virtual ~Texture() noexcept {}

    inline void allocate(Vec<int> const& extents,
                         Texture_Format form = Texture_Format::Rgba) noexcept;

    inline void blit_data(Volume<int> const& vol, Color const* data) noexcept;
    inline void blit_data(Volume<int> const& vol, float const*) noexcept;

    inline Vec<int> allocated_extents() const noexcept;

    virtual inline void fill_data(Volume<int> const& v,Color const c) noexcept;

  private:
    virtual void allocate_(Vec<int> const&, Texture_Format) noexcept = 0;
    virtual void blit_data_(Volume<int> const&, Color const*) noexcept = 0;
    virtual void blit_data_(Volume<int> const&, float const*) noexcept = 0;
    Vec<int> extents_;
  };

  inline void Texture::allocate(Vec<int> const& extents,
                                Texture_Format form) noexcept
  {
    extents_ = extents;
    allocate_(extents, form);
  }
  inline void Texture::blit_data(Volume<int> const& vol,
                                 Color const* data) noexcept
  {
    blit_data_(vol, data);
  }
  inline void Texture::blit_data(Volume<int> const& vol,
                                 float const* data) noexcept
  {
    blit_data_(vol, data);
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
