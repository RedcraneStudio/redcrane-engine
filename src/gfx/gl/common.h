/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../common.h"
#include "glad/glad.h"
namespace redc { namespace gfx { namespace gl
{
  GLenum to_gl_buffer_target(Buffer_Target target);
  GLenum to_gl_usage_hint(Upload_Hint up_h, Usage_Hint us_h);
  GLenum to_gl_data_type(Data_Type type);

  GLenum to_gl_texture_format(Texture_Format format);
  GLenum to_gl_texture_target(Texture_Target target);
  GLenum to_gl_cube_map_texture(Cube_Map_Texture cmt);
  GLenum to_gl_texture_filter(Texture_Filter filter);
  GLenum to_gl_texture_wrap(Texture_Wrap wrap);

  GLenum to_gl_primitive(Primitive_Type type);
  GLenum to_gl_shader_type(Shader_Type shade);

  GLenum to_gl_fbo_binding(Fbo_Binding fbo);
  Fbo_Status from_gl_fbo_status(GLenum status);

  GLenum to_gl_draw_buffer(Draw_Buffer draw);

  GLenum to_gl_attachment_type(Attachment_Type attach);
  GLenum to_gl_attachment(Attachment attach);
} } }
