/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "funcs.h"
#include "../common/debugging.h"
namespace redc
{
#ifdef REDC_USE_OPENGL
  std::vector<Buf_Repr> make_buffers(std::size_t num)
  {
    std::vector<Buf_Repr> bufs;
    if(num > 0)
    {
      bufs.resize(num);

      // We reference the buf member of the first element, assuming they are
      // tightly packed, etc.
      glGenBuffers(num, &bufs[0].buf);
    }
    return bufs;
  }
  Buf_Repr make_buffer()
  {
    Buf_Repr repr;
    glGenBuffers(1, &repr.buf);
    return repr;
  }

  void upload_data(Buf_Repr buf, Buffer_Target target, uint8_t* data,
                   std::size_t length)
  {
    GLenum gltarget = GL_ARRAY_BUFFER;
    switch(target)
    {
    case Buffer_Target::Array:
      gltarget = GL_ARRAY_BUFFER;
      break;
    case Buffer_Target::Element_Array:
      gltarget = GL_ELEMENT_ARRAY_BUFFER;
      break;
    default:
      REDC_UNREACHABLE_MSG("This buffer should not be uploaded to the GPU");
      return;
    }

    glBindBuffer(gltarget, buf.buf);
    glBufferData(gltarget, length, data, GL_STATIC_DRAW);
  }

  void make_textures(std::size_t num, Texture_Repr* repr)
  {
    if(num > 0)
    {
      glGenTextures(num, &repr->tex);
    }
  }

  void set_pixel_store_unpack_alignment(int align)
  {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  }

  void upload_image(Texture_Repr tex, std::vector<uint8_t> const& data,
                    Texture_Target target, Texture_Format dformat,
                    Texture_Format iformat, std::size_t width,
                    std::size_t height, Data_Type type)
  {
    glBindTexture((GLenum) target, tex.tex);
    glTexImage2D((GLenum) target, 0, (GLenum) iformat, width, height, 0,
                 (GLenum) dformat, (GLenum) type, &data[0]);
    glGenerateMipmap((GLenum) target);
  }
  void allocate_texture(Texture_Repr tex, Texture_Target target,
                        Texture_Format iformat, std::size_t width,
                        std::size_t height)
  {
    // Allocate storage for a texture
    glBindTexture((GLenum) target, tex.tex);
    glTexImage2D((GLenum) target, 0, (GLenum) iformat, width, height, 0, GL_RGB,
                 GL_FLOAT, NULL);
  }

  void set_sampler(Texture_Repr tex, Texture_Target target,
                   tinygltf::Sampler const& sampler)
  {
#ifdef REDC_USE_OPENGL_4_5

    // Use direct state access when possible.

    glTextureParameteri(tex.tex, GL_TEXTURE_MAG_FILTER, sampler.magFilter);
    glTextureParameteri(tex.tex, GL_TEXTURE_MIN_FILTER, sampler.minFilter);
    glTextureParameteri(tex.tex, GL_TEXTURE_WRAP_S, sampler.wrapS);
    glTextureParameteri(tex.tex, GL_TEXTURE_WRAP_T, sampler.wrapT);
#else
    glBindTexture((GLenum) target, tex.tex);

    glTexParameteri((GLenum) target, GL_TEXTURE_MAG_FILTER, sampler.magFilter);
    glTexParameteri((GLenum) target, GL_TEXTURE_MIN_FILTER, sampler.minFilter);

    glTexParameteri((GLenum) target, GL_TEXTURE_WRAP_S, sampler.wrapS);
    glTexParameteri((GLenum) target, GL_TEXTURE_WRAP_T, sampler.wrapT);
#endif
  }

  void set_mipmap_level(Texture_Repr tex, Texture_Target target,
                        unsigned int level)
  {
#ifdef REDC_USE_OPENGL_4_5
    glTextureParameteri(tex, GL_TEXTURE_MAX_LEVEL, level);
#else
    glBindTexture((GLenum) target, tex.tex);
    glTexParameteri((GLenum) target, GL_TEXTURE_MAX_LEVEL, level);
#endif
  }

  std::vector<Mesh_Repr> make_mesh_reprs(std::size_t num)
  {
    std::vector<Mesh_Repr> meshs;
    if(num > 0)
    {
      meshs.resize(num);
      glGenVertexArrays(num, &meshs[0].vao);
    }
    return meshs;
  }

  Shader make_shader(Shader_Type type)
  {
    Shader ret;
    ret.type = type;
    ret.repr = {glCreateShader((GLenum) type)};
    return ret;
  }

  Program_Repr make_program()
  {
    Program_Repr ret;
    ret.program = glCreateProgram();
    return ret;
  }

  void use_program(Program_Repr repr)
  {
    glUseProgram(repr.program);
  }

  void upload_shader_source(Shader shade, char const * source,
                            int source_length)
  {
    glShaderSource(shade.repr.shader, 1, &source, &source_length);
  }
  bool compile_shader(Shader shader, std::vector<char>* info_log)
  {
    // Compile
    glCompileShader(shader.repr.shader);

    // Get info log
    if(info_log)
    {
      GLint length = 0;
      glGetShaderiv(shader.repr.shader, GL_INFO_LOG_LENGTH, &length);

      info_log->resize(length);
      if(length > 0)
      {
        glGetShaderInfoLog(shader.repr.shader, length, NULL, &(*info_log)[0]);
      }
    }

    // Return compile status
    GLint val;
    glGetShaderiv(shader.repr.shader, GL_COMPILE_STATUS, &val);
    return val == GL_TRUE;
  }
  void attach_shader(Program_Repr program, Shader_Repr shader)
  {
    glAttachShader(program.program, shader.shader);
  }
  bool link_program(Program_Repr program, std::vector<char>* link_log)
  {
    // Link
    glLinkProgram(program.program);

    // Get info log
    if(link_log)
    {
      GLint length = 0;
      glGetProgramiv(program.program, GL_INFO_LOG_LENGTH, &length);

      link_log->resize(length);
      if(length > 0)
      {
        glGetProgramInfoLog(program.program, length, NULL, &(*link_log)[0]);
      }
    }

    // Return compile status
    GLint val;
    glGetProgramiv(program.program, GL_LINK_STATUS, &val);
    return val == GL_TRUE;
  }

  Attrib_Bind get_attrib_bind(Program_Repr program, std::string const& name)
  {
    Attrib_Bind ret;
    ret.loc = glGetAttribLocation(program.program, name.c_str());
    return ret;
  }
  Param_Bind get_param_bind(Program_Repr program, std::string const& name)
  {
    Param_Bind bind;
    bind.loc = glGetUniformLocation(program.program, name.c_str());
    return bind;
  }

  void destroy_shader(Shader_Repr shader)
  {
    glDeleteShader(shader.shader);
  }
  void destroy_program(Program_Repr program)
  {
    glDeleteProgram(program.program);
  }

  void destroy_bufs(std::size_t num, Buf_Repr* bufs)
  {
    if(num >= 1)
    {
      glDeleteBuffers(num, &bufs->buf);
    }
  }
  void destroy_textures(std::size_t num, Texture_Repr* textures)
  {
    if(num >= 1)
    {
      glDeleteTextures(num, &textures->tex);
    }
  }
  void destroy_meshes(std::size_t num, Mesh_Repr* reprs)
  {
    if(num >= 1)
    {
      glDeleteVertexArrays(num, &reprs->vao);
    }
  }

  void use_mesh(Mesh_Repr mesh)
  {
    glBindVertexArray(mesh.vao);
  }


  // Use this for rendering. It would be nice to pass in some OpenGL state
  // structure so we don't have to redundantly set this information if it has
  // already been set.
  void use_array_accessor(Attrib_Bind bind, Buf_Repr buf, Accessor const& acc)
  {
    // Usually this means the attribute is not actually being used in the shader
    // and can be safely ignored.
    if(bind.loc == -1) return;

    // We have some data
    glBindBuffer(GL_ARRAY_BUFFER, buf.buf);

    // We can't be dealing with matrices of any kind
    REDC_ASSERT((int) acc.attrib_type & 0x10);

    // Bind the buffer to a given attribute, at this point we know exactly
    // what part of the buffer needs to be referenced.
    glVertexAttribPointer(bind.loc, (GLint) acc.attrib_type & 0x0f,
                          (GLenum) acc.data_type, GL_FALSE,
                          acc.stride, (void*) acc.offset);
  }
  void use_element_array_accessor(Buf_Repr buf, Accessor const&)
  {
    // Use this as our element array
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.buf);
  }

  Attrib_Bind bad_attrib_bind()
  {
    Attrib_Bind bind;
    bind.loc = -1;
    return bind;
  }
  bool is_good_attrib_bind(Attrib_Bind bind)
  {
    return bind.loc >= 0;
  }
  Param_Bind bad_param_bind()
  {
    Param_Bind bind;
    bind.loc = -1;
    return bind;
  }
  bool is_good_param_bind(Param_Bind bind)
  {
    return bind.loc >= 0;
  }
  void enable_vertex_attrib_arrays(Mesh_Repr mesh, unsigned int start,
                                   unsigned int end)
  {
#ifdef REDC_USE_OPENGL_4_5
    for(; start != end; ++start)
    {
      glEnableVertexArrayAttrib(mesh.vao, start);
    }
#else
    use_mesh(mesh);
    for(; start != end; ++start)
    {
      glEnableVertexAttribArray(start);
    }
#endif
  }

  void draw_elements(std::size_t count, Data_Type type, Primitive_Type mode,
                     std::size_t offset)
  {
    glDrawElements((GLenum) mode, count, (GLenum) type, (GLvoid*) offset);
  }
  void draw_arrays(std::size_t count, Primitive_Type mode)
  {
    glDrawArrays((GLenum) mode, 0, count);
  }

  void set_parameter(Param_Bind bind, Parameter const& param,
                     int& texture_slot,
                     std::vector<Texture_Repr> const& textures)
  {
    switch(param.type)
    {
    case Param_Type::Byte:
    case Param_Type::UByte:
    case Param_Type::Short:
    case Param_Type::UShort:
      REDC_UNREACHABLE_MSG("Byte and Short parameter types not supported");
      // I suppose we could just set them as integers.
      break;
    case Param_Type::Int:
      set_int_parameter(bind, &param.value.ints[0]);
      break;
    case Param_Type::IVec2:
      set_ivec2_parameter(bind, &param.value.ints[0]);
      break;
    case Param_Type::IVec3:
      set_ivec3_parameter(bind, &param.value.ints[0]);
      break;
    case Param_Type::IVec4:
      set_ivec4_parameter(bind, &param.value.ints[0]);
      break;
    case Param_Type::UInt:
      set_uint_parameter(bind, &param.value.uint);
      break;
    case Param_Type::Bool:
      set_bool_parameter(bind, &param.value.bools[0]);
      break;
    case Param_Type::BVec2:
      set_bvec2_parameter(bind, &param.value.bools[0]);
      break;
    case Param_Type::BVec3:
      set_bvec3_parameter(bind, &param.value.bools[0]);
      break;
    case Param_Type::BVec4:
      set_bvec4_parameter(bind, &param.value.bools[0]);
      break;
    case Param_Type::Float:
      set_float_parameter(bind, &param.value.floats[0]);
      break;
    case Param_Type::Vec2:
      set_vec2_parameter(bind, &param.value.floats[0]);
      break;
    case Param_Type::Vec3:
      set_vec3_parameter(bind, &param.value.floats[0]);
      break;
    case Param_Type::Vec4:
      set_vec4_parameter(bind, &param.value.floats[0]);
      break;
    case Param_Type::Mat2:
      set_mat2_parameter(bind, &param.value.floats[0]);
      break;
    case Param_Type::Mat3:
      set_mat3_parameter(bind, &param.value.floats[0]);
      break;
    case Param_Type::Mat4:
      set_mat4_parameter(bind, &param.value.floats[0]);
      break;
    case Param_Type::Sampler2D:
      // Remember: The value in uint is a reference, however, we are using the
      // index to represent texture unit. We need a cast because samplers must
      // be set with the signed uniform function
      set_int_parameter(bind, &texture_slot);
      // Now activate that texture unit
      glActiveTexture(GL_TEXTURE0 + texture_slot);
      glBindTexture(GL_TEXTURE_2D, textures[param.value.uint].tex);

      // Increment texture slot because we just used the current one
      ++texture_slot;
      break;
    }
  }

  Texture_Format get_attachment_internal_format(Attachment attachment)
  {
    GLuint internal_format;
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

  void make_framebuffers(std::size_t num, Framebuffer_Repr* reprs)
  {
    if(num == 0) return;
    glGenFramebuffers(num, &reprs[0].fbo);
  }
  void destroy_framebuffers(std::size_t num, Framebuffer_Repr* reprs)
  {
    if(num == 0) return;
    glDeleteFramebuffers(num, &reprs[0].fbo);
  }

  void bind_framebuffer(Fbo_Binding binding, Framebuffer_Repr fbo)
  {
    glBindFramebuffer((GLenum) binding, fbo.fbo);
  }

  void unbind_framebuffer()
  {
    glBindFramebuffer((GLenum) Fbo_Binding::Draw, 0);
  }

  GLenum to_gl_attachment(Attachment const& attachment)
  {
    // Convert the attachment to OpenGL terms.
    if(attachment.type == Attachment_Type::Color)
      return ((GLenum) attachment.type) + attachment.i;
    else
      return (GLenum) attachment.type;
  }

  void framebuffer_attach_texture(Fbo_Binding binding, Attachment attachment,
                                  Texture_Target textarget, Texture_Repr tex,
                                  int level)
  {
    glFramebufferTexture2D((GLenum) binding,
                           (GLenum) to_gl_attachment(attachment),
                           (GLenum) textarget,
                           tex.tex, level);
  }
  Fbo_Status check_framebuffer_status(Fbo_Binding target)
  {
    return (Fbo_Status) glCheckFramebufferStatus((GLenum) target);
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

  void make_renderbuffers(std::size_t num, Renderbuffer_Repr* repr)
  {
    if(num == 0) return;
    glGenRenderbuffers(num, &repr->rb);
  }
  void destroy_renderbuffers(std::size_t num, Renderbuffer_Repr* repr)
  {
    if(num == 0) return;
    glDeleteRenderbuffers(num, &repr->rb);
  }
  void bind_renderbuffer(Renderbuffer_Repr repr)
  {
    glBindRenderbuffer(GL_RENDERBUFFER, repr.rb);
  }
  void renderbuffer_storage(Attachment attachment, std::size_t width,
                            std::size_t height)
  {

    glRenderbufferStorage(GL_RENDERBUFFER,
                          (GLenum) get_attachment_internal_format(attachment),
                          width, height);
  }

  void framebuffer_attach_renderbuffer(Fbo_Binding binding,
                                       Attachment attachment,
                                       Renderbuffer_Repr rb)
  {
    glFramebufferRenderbuffer((GLenum) binding, to_gl_attachment(attachment),
                              GL_RENDERBUFFER, rb.rb);
  }

  void set_draw_buffers(std::size_t num, Draw_Buffer* bufs)
  {
    if(num == 0) return;

    std::vector<GLenum> glbufs;
    for(std::size_t i = 0; i < num; ++i)
    {
      if(bufs[i].type == Draw_Buffer_Type::Color)
      {
        glbufs.push_back(((GLenum) bufs[i].type) + bufs[i].i);
      }
      else
      {
        glbufs.push_back((GLenum) bufs[i].type);
      }
    }

    glDrawBuffers(num, &glbufs[0]);
  }

#endif
}
