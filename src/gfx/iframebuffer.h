/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#ifndef REDC_GFX_IFRAMEBUFFER_H
#define REDC_GFX_IFRAMEBUFFER_H
#include "common.h"
#include "ihandle.h"
namespace redc { namespace gfx
{
  struct IRenderbuffer : public IHandle
  {
    virtual ~IRenderbuffer() {}
    virtual void reinitialize() = 0;
    virtual void define_storage(Texture_Format format, Vec<std::size_t> size) = 0;
  };
  struct IFramebuffer : public IHandle
  {
    virtual ~IFramebuffer() {}
    virtual void reinitialize() = 0;
    virtual void attach(Attachment attach, ITexture& texture) = 0;
    // Attach the specific side of a cubemap.
    virtual void attach(Attachment attach, ITexture& texture,
                        Cube_Map_Texture side) = 0;
    virtual void attach(Attachment attach, IRenderbuffer& buf) = 0;

    virtual Fbo_Status status() = 0;
  };
} }
#endif // REDC_GFX_IFRAMEBUFFER_H
