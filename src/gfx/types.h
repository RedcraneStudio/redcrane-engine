/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#ifndef REDC_GFX_TYPES_H
#define REDC_GFX_TYPES_H

#include <cstddef>
#include <array>

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

  // Framebuffer
  struct Framebuffer_Repr
  {
    GLuint fbo;
  };

  struct Renderbuffer_Repr
  {
    GLuint rb;
  };

#elif defined(REDC_USE_DIRECTX)
  struct Attrib_Bind {};
  struct Param_Bind {};
  struct Buf_Repr {};
  struct Texture_Repr {};
  struct Mesh_Repr {};
  struct Shader_Repr {};
  struct Program_Repr {};
  struct Framebuffer_Repr {};
  struct Renderbuffer_Repr {};
#endif

  struct Shader
  {
    Shader_Repr repr;
    Shader_Type type;
  };

  struct Draw_Buffer
  {
    Draw_Buffer_Type type;
    // Value for when type is Color
    uint32_t i;
  };

  struct Attachment
  {
    Attachment_Type type;
    // Value for when type is color
    uint32_t i;
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

  // Type of parameter value
  enum class Param_Type
  {
    Byte, UByte, Short, UShort, Int, UInt, Float, Vec2, Vec3, Vec4, IVec2,
    IVec3, IVec4, Bool, BVec2, BVec3, BVec4, Mat2, Mat3, Mat4, Sampler2D
  };

  // Represents a uniform value
  union Param_Value
  {
    // Signed and unsigned byte
    int8_t byte;
    uint8_t ubyte;

    // Signed and unsigned short.
    short shrt;
    unsigned short ushrt;

    // Unsigned int and sampler **index**
    unsigned int uint;

    // Float, vectors, and matrices
    std::array<float, 16> floats;
    // signed integer and ivec
    std::array<int, 4> ints;
    // bool and bvec
    std::array<bool, 4> bools;
  };

  struct Parameter
  {
    Param_Type type;
    Param_Value value;
  };

}

#endif // Header Guard
