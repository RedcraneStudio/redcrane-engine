/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#ifndef REDC_GFX_IBUFFER_H
#define REDC_GFX_IBUFFER_H
#include <cstdlib>
#include "common.h"
#include "ihandle.h"
namespace redc { namespace gfx
{
  struct IBuffer : public IHandle
  {
    virtual ~IBuffer() {}

    // Data can be null. This function can be used on the same buffer multiple
    // times. The first time it is called, buffer target is used to optimize,
    // just like the OpenGL standard says. Maybe find a way to remove the need
    // to take a buffer target here.
    virtual void allocate(Buffer_Target, std::size_t size, const void* data,
                          Usage_Hint, Upload_Hint) = 0;

    virtual std::size_t allocated_size() = 0;

    // Buffer must have been allocated before this.
    virtual void update(std::size_t offset, std::size_t size,
                        const void* data) = 0;
  };
} }
#endif // REDC_GFX_IBUFFER_H
