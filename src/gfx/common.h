/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#ifndef REDC_GFX_COMMON_H
#define REDC_GFX_COMMON_H

#include <cstddef>
#include <array>

namespace redc { namespace gfx
{
  enum class Buffer_Target
  {
    CPU, Array, Element_Array
  };

  enum class Usage_Hint
  {
    Draw, Read, Copy
  };
  enum class Upload_Hint
  {
    Static, Dynamic, Stream
  };

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

  enum class Texture_Filter
  {
    Nearest,
    Linear,
    Nearest_Mipmap_Nearest,
    Linear_Mipmap_Nearest,
    Nearest_Mipmap_Linear,
    Linear_Mipmap_Linear,
  };
  enum class Texture_Wrap
  {
    Repeat,
    Clamp_To_Edge,
    Mirrored_Repeat,
  };

  enum class Cube_Map_Texture
  {
    Positive_X,
    Negative_X,
    Positive_Y,
    Negative_Y,
    Positive_Z,
    Negative_Z,
  };

  enum class Primitive_Type
  {
    Points, Line, Line_Loop, Line_Strip, Triangles, Triangle_Strip,
    Triangle_Fan
  };

  enum class Shader_Type
  {
    Vertex, Fragment, Geometry
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

  // The most significant nibble is type (1 = vector, 2 = matrix) and the other
  // nibble is the amount. Will be used to quickly distinquish between vector
  // and matrix types.
  enum class Attrib_Type : unsigned char
  {
    Scalar = 0x11, Vec2 = 0x12, Vec3 = 0x13, Vec4 = 0x14, Mat2 = 0x22,
    Mat3 = 0x23, Mat4 = 0x24
  };

  // In OpenGL, attributes are vertex shader input variables.
  using Attrib_Bind = int;

  // Parameters are just uniforms.
  using Param_Bind = int;

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

  struct IBuffer;
  struct Accessor
  {
    // Index into the assets buffers.
    IBuffer* buffer;

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
  enum class Value_Type
  {
    Byte, UByte, Short, UShort, Int, UInt, Float, Vec2, Vec3, Vec4, IVec2,
    IVec3, IVec4, Bool, BVec2, BVec3, BVec4, Mat2, Mat3, Mat4, Sampler2D
  };

  struct ITexture;

  // Represents a uniform value
  union Value
  {
    // Signed and unsigned byte
    int8_t byte;
    uint8_t ubyte;

    // Signed and unsigned short.
    short shrt;
    unsigned short ushrt;

    // Float, vectors, and matrices
    std::array<float, 16> floats;
    // int and ivec
    std::array<int, 4> ints;
    // bool and bvec
    std::array<bool, 4> bools;

    // Sampler 2D
    ITexture* texture;
  };

  struct Typed_Value
  {
    Value_Type type;
    Value value;
  };

  enum class Cull_Side
  {
    Front, Back
  };

  enum class Blend_Policy
  {
    Transparency, Additive,
  };

  using Texture_Slot = unsigned int;

  std::size_t data_type_size(Data_Type ty);
  std::size_t texture_format_num_components(Texture_Format form);

  Texture_Format get_attachment_internal_format(Attachment attachment);
  Draw_Buffer to_draw_buffer(Attachment attachment);

  std::string fbo_status_string(Fbo_Status status);

  Attrib_Bind bad_attrib_bind();
  Param_Bind bad_param_bind();

  bool is_good_attrib_bind(Attrib_Bind);
  bool is_good_param_bind(Param_Bind);

  struct IDriver;
  struct IShader;
  void set_parameter(IDriver& driver, IShader& shader, Param_Bind bind,
                     Typed_Value const& param, Texture_Slot& next_texture_slot);

  inline void set_parameter(IDriver& driver, IShader& shader, Param_Bind bind,
                            Value_Type type, Value const& val,
                            Texture_Slot& texture_slot)
  {
    Typed_Value param;
    param.type = type;
    param.value = val;
    set_parameter(driver, shader, bind, param, texture_slot);
  }

} }

#endif // Header Guard
