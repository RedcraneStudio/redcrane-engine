/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "vec.h"
#include "volume.h"
#include "color.h"
namespace game
{
  struct Texture
  {
    virtual ~Texture() noexcept {}

    inline void allocate(Vec<int> const& extents) noexcept;
    inline void blit_data(Volume<int> const& vol, Color* data) noexcept;

    inline Vec<int> allocated_extents() const noexcept;

  private:
    virtual void allocate_(Vec<int> const& extents) noexcept = 0;
    virtual void blit_data_(Volume<int> const& vol,
                            Color* data) noexcept = 0;
    Vec<int> extents_;
  };

  inline void Texture::allocate(Vec<int> const& extents) noexcept
  {
    extents_ = extents;
    allocate_(extents);
  }
  inline void Texture::blit_data(Volume<int> const& vol,
                                         Color* data) noexcept
  {
    blit_data_(vol, data);
  }

  inline Vec<int> Texture::allocated_extents() const noexcept
  {
    return extents_;
  }
}
