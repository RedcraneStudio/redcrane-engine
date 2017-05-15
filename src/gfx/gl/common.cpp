/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "common.h"
#include "../../common/debugging.h"
namespace redc { namespace gfx { namespace gl
{
  GLenum to_gl_buffer_target(Buffer_Target target)
  {
    switch(target)
    {
    case Buffer_Target::Array:
      return GL_ARRAY_BUFFER;
    case Buffer_Target::Element_Array:
      return GL_ELEMENT_ARRAY_BUFFER;
    default:
      REDC_UNREACHABLE_MSG("Unkown / CPU buffer target cannot be used here");
      // This is as reasonable a default as we will get
      return GL_ARRAY_BUFFER;
    }
  }
  GLenum to_gl_usage_hint(Upload_Hint up_h, Usage_Hint us_h)
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
    return GL_STATIC_DRAW;
  }
  GLenum to_gl_data_type(Data_Type type)
  {
    switch(type)
    {
    case Data_Type::Byte:
      return GL_BYTE;
    case Data_Type::UByte:
      return GL_UNSIGNED_BYTE;
    case Data_Type::Short:
      return GL_SHORT;
    case Data_Type::UShort:
      return GL_UNSIGNED_SHORT;
    case Data_Type::Int:
      return GL_INT;
    case Data_Type::UInt:
      return GL_UNSIGNED_INT;
    case Data_Type::Float:
      return GL_FLOAT;
    case Data_Type::Double:
      return GL_DOUBLE;
    default:
      REDC_UNREACHABLE_MSG("Unknown data type");
      return GL_ZERO;
    }
  }

  GLenum to_gl_texture_format(Texture_Format format)
  {
    switch(format)
    {
    case Texture_Format::Alpha:
      return GL_ALPHA;
    case Texture_Format::Rgb:
      return GL_RGB;
    case Texture_Format::Rgba:
      return GL_RGBA;
    case Texture_Format::Srgb:
      return GL_SRGB;
    case Texture_Format::Srgb_Alpha:
      return GL_SRGB_ALPHA;
    case Texture_Format::Depth:
      return GL_DEPTH;
    case Texture_Format::Depth_Stencil:
      return GL_DEPTH_STENCIL;
    case Texture_Format::Stencil:
      return GL_STENCIL;
    case Texture_Format::Red:
      return GL_RED;
    case Texture_Format::Rgba32F:
      return GL_RGBA32F;
    default:
      REDC_UNREACHABLE_MSG("Unknown texture format");
      return GL_ZERO;
    }
  }
  GLenum to_gl_texture_target(Texture_Target target)
  {
    switch(target)
    {
    case Texture_Target::Tex_2D:
      return GL_TEXTURE_2D;
    case Texture_Target::Cube_Map:
      return GL_TEXTURE_CUBE_MAP;
    default:
      REDC_UNREACHABLE_MSG("Unknown texture target");
      return GL_TEXTURE_2D;
    }
  }
  GLenum to_gl_cube_map_texture(Cube_Map_Texture cmt)
  {
    switch(cmt)
    {
    case Cube_Map_Texture::Positive_X:
      return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    case Cube_Map_Texture::Negative_X:
      return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
    case Cube_Map_Texture::Positive_Y:
      return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
    case Cube_Map_Texture::Negative_Y:
      return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
    case Cube_Map_Texture::Positive_Z:
      return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
    case Cube_Map_Texture::Negative_Z:
      return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
    default:
      REDC_UNREACHABLE_MSG("Unknown cube map side");
      return GL_ZERO;
    }
  }
  GLenum to_gl_texture_filter(Texture_Filter filter)
  {
    switch(filter)
    {
    case Texture_Filter::Nearest:
      return GL_NEAREST;
    case Texture_Filter::Linear:
      return GL_LINEAR;
    case Texture_Filter::Nearest_Mipmap_Nearest:
      return GL_NEAREST_MIPMAP_NEAREST;
    case Texture_Filter::Linear_Mipmap_Nearest:
      return GL_LINEAR_MIPMAP_NEAREST;
    case Texture_Filter::Nearest_Mipmap_Linear:
      return GL_NEAREST_MIPMAP_LINEAR;
    case Texture_Filter::Linear_Mipmap_Linear:
      return GL_LINEAR_MIPMAP_LINEAR;
    default:
      REDC_UNREACHABLE_MSG("Unknown texture filter");
      return GL_ZERO;
    }
  }
  GLenum to_gl_texture_wrap(Texture_Wrap wrap)
  {
    switch(wrap)
    {
    case Texture_Wrap::Repeat:
      return GL_REPEAT;
    case Texture_Wrap::Clamp_To_Edge:
      return GL_CLAMP_TO_EDGE;
    case Texture_Wrap::Mirrored_Repeat:
      return GL_MIRRORED_REPEAT;
    default:
      REDC_UNREACHABLE_MSG("Unknown texture wrapping type");
      return GL_ZERO;
    }
  }
  GLenum to_gl_primitive(Primitive_Type type)
  {
    switch(type)
    {
    case Primitive_Type::Points:
      return GL_POINTS;
    case Primitive_Type::Line:
      return GL_LINE;
    case Primitive_Type::Line_Loop:
      return GL_LINE_LOOP;
    case Primitive_Type::Line_Strip:
      return GL_LINE_STRIP;
    case Primitive_Type::Triangles:
      return GL_TRIANGLES;
    case Primitive_Type::Triangle_Strip:
      return GL_TRIANGLE_STRIP;
    case Primitive_Type::Triangle_Fan:
        return GL_TRIANGLE_FAN;
    default:
      REDC_UNREACHABLE_MSG("Unknown primitive type");
      return GL_TRIANGLES;
    }
  }
  GLenum to_gl_shader_type(Shader_Type shade)
  {
    switch(shade)
    {
    case Shader_Type::Vertex:
      return GL_VERTEX_SHADER;
    case Shader_Type::Fragment:
      return GL_FRAGMENT_SHADER;
    case Shader_Type::Geometry:
      return GL_GEOMETRY_SHADER;
    default:
      REDC_UNREACHABLE_MSG("Unknown shader type");
      return GL_ZERO;
    }
  }

  GLenum to_gl_attachment_type(Attachment_Type attach)
  {
    switch(attach)
    {
    case Attachment_Type::Color:
      return GL_COLOR_ATTACHMENT0;
    case Attachment_Type::Depth:
      return GL_DEPTH_ATTACHMENT;
    case Attachment_Type::Stencil:
      return GL_STENCIL_ATTACHMENT;
    case Attachment_Type::Depth_Stencil:
      return GL_DEPTH_STENCIL_ATTACHMENT;
    default:
      REDC_UNREACHABLE_MSG("Unknown framebuffer attachment");
      return GL_NONE;
    }
  }
  GLenum to_gl_fbo_binding(Fbo_Binding fbo)
  {
    switch(fbo)
    {
    case Fbo_Binding::Draw:
      return GL_DRAW_FRAMEBUFFER;
    case Fbo_Binding::Read:
      return GL_READ_FRAMEBUFFER;
    default:
      REDC_UNREACHABLE_MSG("Unknown framebuffer binding");
      return GL_NONE;
    }
  }
  Fbo_Status from_gl_fbo_status(GLenum status)
  {
    switch(status)
    {
    case GL_FRAMEBUFFER_COMPLETE:
      return Fbo_Status::Complete;
    case GL_FRAMEBUFFER_UNDEFINED:
      return Fbo_Status::Undefined;
    case GL_FRAMEBUFFER_UNSUPPORTED:
      return Fbo_Status::Unsupported;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
      return Fbo_Status::Incomplete_Attachment;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
      return Fbo_Status::Incomplete_Missing_Attachment;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
      return Fbo_Status::Incomplete_Draw_Buffer;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
      return Fbo_Status::Incomplete_Read_Buffer;
    default:
      return Fbo_Status::Other;
    }
  }

  GLenum to_gl_draw_buffer(Draw_Buffer draw)
  {
    switch(draw.type)
    {
    case Draw_Buffer_Type::Back_Left:
      return GL_BACK_LEFT;
    case Draw_Buffer_Type::Back_Right:
      return GL_BACK_RIGHT;
    case Draw_Buffer_Type::Front_Left:
      return GL_FRONT_LEFT;
    case Draw_Buffer_Type::Front_Right:
      return GL_FRONT_RIGHT;
    case Draw_Buffer_Type::Color:
      return GL_COLOR_ATTACHMENT0 + draw.i;
    default:
      REDC_UNREACHABLE_MSG("Unknown draw buffer type");
    case Draw_Buffer_Type::None:
      return GL_NONE;
    }
  }

  GLenum to_gl_attachment(Attachment attach)
  {
    switch(attach.type)
    {
    case Attachment_Type::Color:
      return GL_COLOR_ATTACHMENT0 + attach.i;
    case Attachment_Type::Depth:
      return GL_DEPTH_ATTACHMENT;
    case Attachment_Type::Stencil:
      return GL_STENCIL_ATTACHMENT;
    case Attachment_Type::Depth_Stencil:
      return GL_DEPTH_STENCIL_ATTACHMENT;
    default:
      REDC_UNREACHABLE_MSG("Unknown attachment type");
      return GL_NONE;
    };
  }

} } }
