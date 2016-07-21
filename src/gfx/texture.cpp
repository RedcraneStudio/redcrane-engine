/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "texture.h"
namespace redc
{
  void Texture::allocate(Vec<int> const& extents, Texture_Format form,
                         Texture_Target type) noexcept
  {
    extents_ = extents;
    allocate_(extents, form, type);
  }
}
