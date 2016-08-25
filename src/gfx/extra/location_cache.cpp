/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "location_cache.h"
namespace redc { namespace gfx
{
  int Location_Cache::operator()(std::string const& str) const noexcept
  {
    auto res = cache_.emplace(str, shader_->get_location(str));
    return std::get<0>(res)->second;
  }
} }
