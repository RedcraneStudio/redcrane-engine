/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "itexture.h"
namespace redc { namespace gfx
{
  void ITexture::allocate(Vec<std::size_t> const& extents, Texture_Format form,
                          Texture_Target type)
  {
    extents_ = extents;
    target_ = type;
    allocate_(extents, form, type);
  }
} }
