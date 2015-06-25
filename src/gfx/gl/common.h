/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../mesh.h"
#include "glad/glad.h"
#include "../idriver.h"
namespace game { namespace gfx { namespace gl
{
  GLenum get_gl_hint(Upload_Hint up_h, Usage_Hint us_h) noexcept;
  GLenum get_gl_primitive(Primitive_Type type) noexcept;
  GLenum get_gl_buffer_format(Buffer_Format format) noexcept;

  GLenum get_gl_pixel_format(Framebuffer format) noexcept;
} } }

