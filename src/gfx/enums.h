/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#ifndef REDC_GFX_ENUMS_H
#define REDC_GFX_ENUMS_H


#if defined(REDC_USE_OPENGL)
#include "glad/glad.h"
#endif

namespace redc
{
#if defined(REDC_USE_OPENGL)
  enum class Data_Type : GLenum
  {
    Byte = GL_BYTE, UByte = GL_UNSIGNED_BYTE, Short = GL_SHORT,
    UShort = GL_UNSIGNED_SHORT, Int = GL_INT, UInt = GL_UNSIGNED_INT,
    Float = GL_FLOAT, Double = GL_DOUBLE
  };

  enum class Texture_Format : GLenum
  {
    Alpha = GL_ALPHA, Rgb = GL_RGB, Rgba = GL_RGBA
  };
  enum class Texture_Target : GLenum
  {
    Tex_2D = GL_TEXTURE_2D, Cube_Map = GL_TEXTURE_CUBE_MAP
  };

  enum class Render_Mode : GLenum
  {
    Points = GL_POINTS, Lines = GL_LINE, Line_Loop = GL_LINE_LOOP,
    Line_Strip = GL_LINE_STRIP, Triangles = GL_TRIANGLES,
    Triangle_Strip = GL_TRIANGLE_STRIP, Triangle_Fan = GL_TRIANGLE_FAN
  };

  enum class Shader_Type : GLenum
  {
    Vertex = GL_VERTEX_SHADER, Fragment = GL_FRAGMENT_SHADER
  };
#else
  enum class Data_Type
  {
    Byte, UByte, Short, UShort, Int, UInt, Float, Double
  };

  enum class Texture_Format
  {
    Alpha, Rgb, Rgba
  };
  enum class Texture_Target
  {
    Tex_2D, Cube_Map
  };

  enum class Render_Mode
  {
    Points, Lines, Line_Loop, Line_Strip, Triangles, Triangle_Strip,
    Triangle_Fan
  };

  enum class Shader_Type
  {
    Vertex, Fragment
  };

#endif
}

#endif // Header guard
