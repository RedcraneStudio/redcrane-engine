/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
namespace redc { namespace ui
{
  template <class T, class U>
  inline std::shared_ptr<T> as(std::shared_ptr<U> const& v)
  {
    return std::dynamic_pointer_cast<T, U>(v);
  }
} }
