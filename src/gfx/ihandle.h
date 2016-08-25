/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#ifndef REDC_GFX_IHANDLE_H
#define REDC_GFX_IHANDLE_H
namespace redc { namespace gfx
{
  struct IHandle
  {
    virtual ~IHandle() {}

    // Complete tear down and re-initialization. Useful for when repurpose an
    // object for some other use without destroying references to the handle
    // itself. Possibly good for implementation of a LOD. Does not preserve the
    // original contents. For example you'll have to reload a shaders or
    // reupload textures or meshes, etc. Having this as the only interface means
    // the handle will always be in a consistent usable state.
    virtual void reinitialize() = 0;
  };
} }
#endif // Header Guard
