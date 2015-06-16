/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "texture_composite.h"
namespace game
{
  Texture_Composite::Texture_Composite(Texture_Composite&& rhs) noexcept
    : ts_(std::move(rhs.ts_)) {}
  Texture_Composite& Texture_Composite::
  operator=(Texture_Composite&& rhs) noexcept
  {
    ts_ = std::move(rhs.ts_);
    return *this;
  }
  void Texture_Composite::allocate_(Vec<int> const& extents) noexcept
  {
    for(auto& ptr : ts_)
    {
      ptr->allocate(extents);
    }
  }
  void Texture_Composite::
  blit_data_(Volume<int> const& vol, Color const* data) noexcept
  {
    for(auto& ptr : ts_)
    {
      ptr->blit_data(vol, data);
    }
  }
}
