/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "common.h"
namespace redc { namespace gfx { namespace gl
{
  GLenum get_gl_hint(Upload_Hint up_h, Usage_Hint us_h) noexcept
  {
    if(up_h == Upload_Hint::Static && us_h == Usage_Hint::Draw)
    { return GL_STATIC_DRAW; }
    else if(up_h == Upload_Hint::Static && us_h == Usage_Hint::Read)
    { return GL_STATIC_READ; }
    else if(up_h == Upload_Hint::Static && us_h == Usage_Hint::Copy)
    { return GL_STATIC_COPY; }
    else if(up_h == Upload_Hint::Dynamic && us_h == Usage_Hint::Draw)
    { return GL_DYNAMIC_DRAW; }
    else if(up_h == Upload_Hint::Dynamic && us_h == Usage_Hint::Read)
    { return GL_DYNAMIC_READ; }
    else if(up_h == Upload_Hint::Dynamic && us_h == Usage_Hint::Copy)
    { return GL_DYNAMIC_COPY; }
    else if(up_h == Upload_Hint::Stream && us_h == Usage_Hint::Draw)
    { return GL_STREAM_DRAW; }
    else if(up_h == Upload_Hint::Stream && us_h == Usage_Hint::Read)
    { return GL_STREAM_READ; }
    else if(up_h == Upload_Hint::Stream && us_h == Usage_Hint::Copy)
    { return GL_STREAM_COPY; }

    // This shouldn't happen at all, but just in case we pick a reasonable
    // default.
    return GL_DYNAMIC_DRAW;
  }

  GLenum get_gl_primitive(Primitive_Type type) noexcept
  {
    switch(type)
    {
      case Primitive_Type::Line:
        return GL_LINES;
      case Primitive_Type::Triangle:
        return GL_TRIANGLES;
      case Primitive_Type::Triangle_Fan:
        return GL_TRIANGLE_FAN;
    }

    // This should never return
    return 0;
  }

  GLenum get_gl_pixel_format(Framebuffer) noexcept
  {
    return GL_DEPTH_COMPONENT;
  }
} } }
