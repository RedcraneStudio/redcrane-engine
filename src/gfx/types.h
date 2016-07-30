/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#ifndef REDC_GFX_TYPES_H
#define REDC_GFX_TYPES_H

#include <cstddef>
#include "enums.h"

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

  struct Shader
  {
    Shader_Repr repr;
    Shader_Type type;
  };

  using Buf_Ref = std::size_t;
  struct Accessor
  {
    // Index into the assets buffers.
    Buf_Ref buf_i;

    // Number of attributes
    std::size_t count;

    // In bytes
    std::size_t offset;
    std::size_t stride;

    // Type of each element e.g. float
    Data_Type data_type;
    // What kind of attribute e.g. 4-component vector.
    Attrib_Type attrib_type;
  };

}

#endif // Header Guard
