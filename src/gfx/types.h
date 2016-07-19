/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#ifndef REDC_GFX_TYPES_H
#define REDC_GFX_TYPES_H

#if defined(REDC_USE_OPENGL)
#include "glad/glad.h"
#endif

namespace redc
{
#if defined(REDC_USE_OPENGL)
  // In OpenGL, attributes are vertex shader input variables.
  struct Attrib_Bind
  {
    GLint loc;
  };

  // Parameters are just uniforms.
  struct Param_Bind
  {
    GLint loc;
  };

  // A VBO
  struct Buf_Repr
  {
    GLuint buf;
  };

  // A texture object
  struct Texture_Repr
  {
    GLuint tex;
  };

  // A VAO
  struct Mesh_Repr
  {
    GLuint vao;
  };

  // Shader
  struct Shader_Repr
  {
    GLuint shader;
  };

  // Program
  struct Program_Repr
  {
    GLuint program;
  };

#elif defined(REDC_USE_DIRECTX)
  struct Attrib_Bind {};
  struct Param_Bind {};
  struct Buf_Repr {};
  struct Texture_Repr {};
  struct Mesh_Repr {};
  struct Shader_Repr {};
  struct Program_Repr {};
#endif
}

#endif // Header Guard
