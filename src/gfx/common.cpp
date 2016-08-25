/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#include "common.h"
#include "../common/debugging.h"
#include "idriver.h"
#include "ishader.h"
namespace redc { namespace gfx
{
  std::size_t data_type_size(Data_Type ty)
  {
    switch(ty)
    {
    case Data_Type::UByte:
    case Data_Type::Byte:
      return sizeof(char);

    case Data_Type::UShort:
    case Data_Type::Short:
      return sizeof(short);

    case Data_Type::UInt:
    case Data_Type::Int:
      return sizeof(int);

    default:
      REDC_UNREACHABLE_MSG("Unknown data type");
      return 0;
    }
  }
  std::size_t texture_format_num_components(Texture_Format form)
  {
    switch(form)
    {
    case Texture_Format::Alpha:
    case Texture_Format::Depth:
    case Texture_Format::Stencil:
    case Texture_Format::R8:
      return 1;
    case Texture_Format::Depth_Stencil:
      return 2;
    case Texture_Format::Rgb:
    case Texture_Format::Srgb:
      return 3;
    case Texture_Format::Rgba:
    case Texture_Format::Rgba32F:
    case Texture_Format::Srgb_Alpha:
      return 4;
    default:
      REDC_UNREACHABLE_MSG("Unknown texture format");
      return 0;
    }
  }

  Texture_Format get_attachment_internal_format(Attachment attachment)
  {
    switch(attachment.type)
    {
    case Attachment_Type::Color:
      return Texture_Format::Rgba32F;
    case Attachment_Type::Depth:
      return Texture_Format::Depth;
    case Attachment_Type::Depth_Stencil:
      return Texture_Format::Depth_Stencil;
    case Attachment_Type::Stencil:
      return Texture_Format::Stencil;
    default:
      REDC_UNREACHABLE_MSG("Unknown attachment type");
      return Texture_Format::Rgba;
    }
  }
  Draw_Buffer to_draw_buffer(Attachment attachment)
  {
    Draw_Buffer ret;
    switch(attachment.type)
    {
    case Attachment_Type::Color:
      ret.type = Draw_Buffer_Type::Color;
      ret.i = attachment.i;
      break;
    default:
      ret.type = Draw_Buffer_Type::None;
      break;
    }
    return ret;
  }

  std::string fbo_status_string(Fbo_Status status)
  {
    switch(status)
    {
    case Fbo_Status::Complete:
      return "No error";
    case Fbo_Status::Undefined:
      return "Framebuffer undefined";
    case Fbo_Status::Unsupported:
      return "Framebuffer unsupported";
    case Fbo_Status::Incomplete_Attachment:
      return "Framebuffer incomplete attachment";
    case Fbo_Status::Incomplete_Missing_Attachment:
      return "Framebuffer missing attachment";
    case Fbo_Status::Incomplete_Draw_Buffer:
      return "Framebuffer incomplete draw buffer";
    case Fbo_Status::Incomplete_Read_Buffer:
      return "Framebuffer incomplete read buffer";
    default:
      return "Unknown error";
    }
  }
  void set_parameter(IDriver& driver, IShader& shader, Param_Bind bind,
                     Typed_Value const& param, Texture_Slot& next_texture_slot)
  {
    switch(param.type)
    {
    case Value_Type::Byte:
    case Value_Type::UByte:
    case Value_Type::Short:
    case Value_Type::UShort:
      REDC_UNREACHABLE_MSG("Byte and Short parameter types not supported");
      // I suppose we could just set them as integers.
      break;
    case Value_Type::Int:
    case Value_Type::UInt:
      shader.set_integer(bind, param.value.ints[0]);
      break;
    case Value_Type::IVec2:
      shader.set_ivec2(bind, &param.value.ints[0]);
      break;
    case Value_Type::IVec3:
      shader.set_ivec3(bind, &param.value.ints[0]);
      break;
    case Value_Type::IVec4:
      shader.set_ivec4(bind, &param.value.ints[0]);
      break;
    case Value_Type::Bool:
      shader.set_bool(bind, param.value.bools[0]);
      break;
    case Value_Type::BVec2:
      shader.set_bvec2(bind, &param.value.bools[0]);
      break;
    case Value_Type::BVec3:
      shader.set_bvec3(bind, &param.value.bools[0]);
      break;
    case Value_Type::BVec4:
      shader.set_bvec4(bind, &param.value.bools[0]);
      break;
    case Value_Type::Float:
      shader.set_float(bind, param.value.floats[0]);
      break;
    case Value_Type::Vec2:
      shader.set_vec2(bind, &param.value.floats[0]);
      break;
    case Value_Type::Vec3:
      shader.set_vec3(bind, &param.value.floats[0]);
      break;
    case Value_Type::Vec4:
      shader.set_vec4(bind, &param.value.floats[0]);
      break;
    case Value_Type::Mat2:
      shader.set_mat2(bind, &param.value.floats[0]);
      break;
    case Value_Type::Mat3:
      shader.set_mat3(bind, &param.value.floats[0]);
      break;
    case Value_Type::Mat4:
      shader.set_mat4(bind, &param.value.floats[0]);
      break;
    case Value_Type::Sampler2D:
      // Bind the texture at the texture slot
      driver.active_texture(next_texture_slot);
      driver.bind_texture(*param.value.texture, param.value.texture->target());

      // Use that texture slot
      shader.set_integer(bind, next_texture_slot++);
      break;
    }
  }

  Attrib_Bind bad_attrib_bind()
  {
    return (Attrib_Bind) -1;
  }
  Param_Bind bad_param_bind()
  {
    return (Param_Bind) -1;
  }

  bool is_good_attrib_bind(Attrib_Bind bind)
  {
    return bind >= 0;
  }
  bool is_good_param_bind(Param_Bind bind)
  {
    return bind >= 0;
  }

} }
