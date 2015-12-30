/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "texture.h"
namespace game
{
  void Texture::allocate(Vec<int> const& extents, Image_Format form,
                         Image_Type type) noexcept
  {
    extents_ = extents;
    allocate_(extents, form, type);
  }
}
