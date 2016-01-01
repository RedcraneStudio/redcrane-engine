/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <uv.h>

namespace redc
{
  // libuv allocator.
  inline void alloc(uv_handle_t* handle, size_t ssize, uv_buf_t* buf)
  {
    buf->base = new char[ssize];
    buf->len = ssize;
  }
}
