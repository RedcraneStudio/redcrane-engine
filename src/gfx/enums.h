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
  enum class Buffer_Target
  {
    CPU, Array, Element_Array
  };

  enum class Usage_Hint : GLenum
  {
    Draw, Read, Copy
  };
  enum class Upload_Hint : GLenum
  {
    Static, Dynamic, Stream
  };

#if defined(REDC_USE_OPENGL)
  enum class Data_Type : GLenum
  {
    Byte = GL_BYTE, UByte = GL_UNSIGNED_BYTE, Short = GL_SHORT,
    UShort = GL_UNSIGNED_SHORT, Int = GL_INT, UInt = GL_UNSIGNED_INT,
    Float = GL_FLOAT, Double = GL_DOUBLE
  };

  enum class Texture_Format : GLenum
  {
    Alpha = GL_ALPHA, Rgb = GL_RGB, Rgba = GL_RGBA, Srgb = GL_SRGB,
    Srgb_Alpha = GL_SRGB_ALPHA, Depth = GL_DEPTH_COMPONENT,
    Depth_Stencil = GL_DEPTH_STENCIL, Stencil = GL_STENCIL_INDEX,

    R8 = GL_R8,
    Rgba32F = GL_RGBA32F
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

  enum class Draw_Buffer_Type
  {
    None = GL_NONE,
    Back_Left = GL_BACK_LEFT,
    Back_Right = GL_BACK_RIGHT,
    Front_Left = GL_FRONT_LEFT,
    Front_Right = GL_FRONT_RIGHT,
    Color = GL_COLOR_ATTACHMENT0
  };

  enum class Attachment_Type
  {
    Color = GL_COLOR_ATTACHMENT0, Depth = GL_DEPTH_ATTACHMENT,
    Stencil = GL_STENCIL_ATTACHMENT, Depth_Stencil = GL_DEPTH_STENCIL_ATTACHMENT
  };

  enum class Fbo_Binding
  {
    Draw = GL_DRAW_FRAMEBUFFER, Read = GL_READ_FRAMEBUFFER
  };

  enum class Fbo_Status
  {
    Complete = GL_FRAMEBUFFER_COMPLETE,
    Undefined = GL_FRAMEBUFFER_UNDEFINED,
    Unsupported = GL_FRAMEBUFFER_UNSUPPORTED,
    Incomplete_Attachment = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,
    Incomplete_Missing_Attachment =GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,
    Incomplete_Draw_Buffer = GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER,
    Incomplete_Read_Buffer = GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER,
    Other
  };

#else

  enum class Data_Type
  {
    Byte, UByte, Short, UShort, Int, UInt, Float, Double
  };

  enum class Texture_Format
  {
    Alpha, Rgb, Rgba, Srgb, Srgb_Alpha, Depth, Depth_Stencil, Stencil, R8,
    Rgba32F
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

  enum class Draw_Buffer_Type
  {
    None, Back_Left, Back_Right, Front_Left, Front_Right, Color
  };
  enum class Attachment_Type
  {
    Color, Depth, Stencil, Depth_Stencil
  };

  enum class Fbo_Binding
  {
    Draw, Read
  };
  enum class Fbo_Status
  {
    Complete, Undefined, Unsupported, Incomplete_Attachment,
    Incomplete_Missing_Attachment, Incomplete_Draw_Buffer,
    Incomplete_Read_Buffer, Other
  };

#endif

  // The most significant nibble is type (1 = vector, 2 = matrix) and the other
  // nibble is the amount. Will be used to quickly distinquish between vector
  // and matrix types.
  enum class Attrib_Type : unsigned char
  {
    Scalar = 0x11, Vec2 = 0x12, Vec3 = 0x13, Vec4 = 0x14, Mat2 = 0x22,
    Mat3 = 0x23, Mat4 = 0x24
  };


}

#endif // Header guard
