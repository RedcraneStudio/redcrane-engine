/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "scene.h"
#include "../common/debugging.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <boost/variant/get.hpp>

namespace redc
{

#ifdef REDC_USE_OPENGL
  std::vector<Buf> make_buffers(std::size_t num)
  {
    std::vector<Buf> bufs;
    if(num > 0)
    {
      bufs.resize(num);

      // We reference the buf member of the first element, assuming they are
      // tightly packed, etc.
      glGenBuffers(num, &bufs[0].buf);
    }
    return bufs;
  }

  void upload_data(Buf buf, std::vector<uint8_t> const& data, std::size_t off,
                   std::size_t length, Buffer_Target target)
  {
    glBindBuffer((GLenum) target, buf.buf);
    glBufferData((GLenum) target, length, &data[0] + off, GL_STATIC_DRAW);
  }

  std::vector<Texture_Repr> make_textures(std::size_t num)
  {
    std::vector<Texture_Repr> texs;
    if(num > 0)
    {
      texs.resize(num);
      glGenTextures(num, &texs[0].tex);
    }
    return texs;
  }

  void set_pixel_store_unpack_alignment(int align)
  {
    glPixelStorei(GL_UNPACK_ALIGNMENT, align);
  }

  void upload_image(Texture_Repr tex, std::vector<uint8_t> const& data,
                    Texture_Target target, Texture_Format format,
                    std::size_t width, std::size_t height, Data_Type type)
  {
    glBindTexture((GLenum) target, tex.tex);
    glTexImage2D((GLenum) target, 0, (GLenum) format, width, height, 0,
                 (GLenum) format, (GLenum) type, &data[0]);
    glGenerateMipmap((GLenum) target);
  }

  void set_sampler(Texture_Repr tex, Texture_Target target)
  {
    // Can we avoid rebinding?
    glBindTexture((GLenum) target, tex.tex);
    glTextureParameterf((GLenum) target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameterf((GLenum) target, GL_TEXTURE_MIN_FILTER,
                        GL_NEAREST_MIPMAP_LINEAR);

    glTextureParameterf((GLenum) target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameterf((GLenum) target, GL_TEXTURE_WRAP_T, GL_REPEAT);

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

      GLsizei actual_length = 0;
      info_log->resize(length);
      glGetShaderInfoLog(shader.repr.shader, length, &actual_length,
                         &(*info_log)[0]);
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
      GLint length;
      glGetProgramiv(program.program, GL_INFO_LOG_LENGTH, &length);

      link_log->resize(length);
      glGetProgramInfoLog(program.program, length, NULL, &(*link_log)[0]);
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

  void destroy_bufs(std::size_t num, Buf* bufs)
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
  void use_array_accessor(Attrib_Bind bind, Buf buf, Accessor const& acc)
  {
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
  void use_element_array_accessor(Buf buf, Accessor const&)
  {
    // Use this as our element array
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.buf);
  }

  // = Uniform functions!

  // == Int functions
  inline void set_uint_parameter(Param_Bind bind, unsigned int const* val)
  {
    glUniform1uiv(bind.loc, 1, val);
  }
  inline void set_int_parameter(Param_Bind bind, int const* val)
  {
    glUniform1iv(bind.loc, 1, val);
  }
  inline void set_ivec2_parameter(Param_Bind bind, int const* val)
  {
    glUniform2iv(bind.loc, 1, val);
  }
  inline void set_ivec3_parameter(Param_Bind bind, int const* val)
  {
    glUniform3iv(bind.loc, 1, val);
  }
  inline void set_ivec4_parameter(Param_Bind bind, int const* val)
  {
    glUniform4iv(bind.loc, 1, val);
  }

  // == Bool functions
  inline void set_bool_parameter(Param_Bind bind, bool const* val)
  {
    glUniform1ui(bind.loc, *val);
  }
  inline void set_bvec2_parameter(Param_Bind bind, bool const* val)
  {
    glUniform2ui(bind.loc, val[0], val[1]);
  }
  inline void set_bvec3_parameter(Param_Bind bind, bool const* val)
  {
    glUniform3ui(bind.loc, val[0], val[1], val[2]);
  }
  inline void set_bvec4_parameter(Param_Bind bind, bool const* val)
  {
    glUniform4ui(bind.loc, val[0], val[1], val[2], val[3]);
  }

  // == Float functions
  inline void set_float_parameter(Param_Bind bind, float const* vec)
  {
    glUniform1fv(bind.loc, 1, vec);
  }
  inline void set_vec2_parameter(Param_Bind bind, float const* vec)
  {
    glUniform2fv(bind.loc, 1, vec);
  }
  inline void set_vec3_parameter(Param_Bind bind, float const* vec)
  {
    glUniform3fv(bind.loc, 1, vec);
  }
  inline void set_vec4_parameter(Param_Bind bind, float const* vec)
  {
    glUniform4fv(bind.loc, 1, vec);
  }
  inline void set_mat2_parameter(Param_Bind bind, float const* vec)
  {
    glUniformMatrix2fv(bind.loc, 1, GL_FALSE, vec);
  }
  inline void set_mat3_parameter(Param_Bind bind, float const* vec)
  {
    glUniformMatrix3fv(bind.loc, 1, GL_FALSE, vec);
  }
  inline void set_mat4_parameter(Param_Bind bind, float const* vec)
  {
    glUniformMatrix4fv(bind.loc, 1, GL_FALSE, vec);
  }

  void enable_vertex_attrib_arrays(Mesh_Repr mesh, unsigned int start,
                                unsigned int end)
  {
// OPENGL VERSION < 4.5
    use_mesh(mesh);
    for(; start != end; ++start)
    {
      glEnableVertexAttribArray(start);
    }
// OPENGL VERSION 4.5
/*
    for(; start != end; ++start)
    {
      glEnableVertexArrayAttrib(mesh.vao, start);
    }
*/
  }

  void draw_elements(std::size_t count, Data_Type type, Render_Mode mode,
                     std::size_t offset)
  {
    glDrawElements((GLenum) mode, count, (GLenum) type, (GLvoid*) offset);
  }
  void draw_arrays(std::size_t count, Render_Mode mode)
  {
    glDrawArrays((GLenum) mode, 0, count);
  }

#endif

  // = Helper functions

  void set_parameter(Param_Bind bind, Parameter const& param,
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
      set_int_parameter(bind, (int*) &param.value.uint);
      // Now activate that texture unit
      glActiveTexture(GL_TEXTURE0 + param.value.uint);
      glBindTexture(GL_TEXTURE_2D, textures[param.value.uint].tex);
      break;
    }
  }

  inline void set_parameter(Param_Bind bind, Param_Type type,
                            Param_Value const& val,
                            std::vector<Texture_Repr> const& textures)
  {
    Parameter param;
    param.type = type;
    param.value = val;
    set_parameter(bind, param, textures);
  }

  template <class Bind_T, unsigned int Which_N, class Semantic_T>
  Bind_T get_semantic_bind(
    std::unordered_map<std::string, Param_Decl> const& parameters,
    Semantic_T attrib_semantic)
  {
    auto param_find = std::find_if(parameters.begin(),
                                   parameters.end(),
    [&](auto const& tech_param)
    {
      if(tech_param.second.semantic == boost::none) return false;
      if(tech_param.second.semantic.value().which() != Which_N) return false;

      auto const& param_semantic =
              boost::get<Semantic_T>(tech_param.second.semantic.value());

      // If these are equal we found our parameter
      return param_semantic == attrib_semantic;
    });

    // We should have found a parameter
    REDC_ASSERT(param_find != parameters.end());

    // It should be an attribute bind
    REDC_ASSERT_MSG(param_find->second.bind.which() == Which_N,
                    "Parameter '%' must be referencing an attribute",
                    param_find->first);

    // Now get the bind
    return boost::get<Bind_T>(param_find->second.bind);
  }

  Attrib_Bind get_attrib_semantic_bind(Technique const& tech,
                                       Attrib_Semantic attrib_semantic)
  {
    return get_semantic_bind<Attrib_Bind, 1>(tech.parameters,
                                             attrib_semantic);
  }

  Param_Bind get_param_semantic_bind(Technique const& tech,
                                     Param_Semantic param_semantic)
  {
    return get_semantic_bind<Param_Bind, 0>(tech.parameters,
                                                param_semantic);

  }

  template <class... Args>
  std::size_t find_string_index(std::vector<std::string> strs,
                                std::string str,
                                Args&&... msg_args)
  {
    auto find_res = std::find(strs.begin(), strs.end(), str);
    REDC_ASSERT_MSG(find_res != strs.end(), std::forward<Args>(msg_args)...);
    return find_res - strs.begin();
  }

  // = Conversion functions (from tinygltfloader to our enums).

  Data_Type to_data_type(int val)
  {
    switch(val)
    {
    case TINYGLTF_COMPONENT_TYPE_BYTE:
      return Data_Type::Byte;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
      return Data_Type::UByte;
    case TINYGLTF_COMPONENT_TYPE_SHORT:
      return Data_Type::Short;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
      return Data_Type::UShort;
    case TINYGLTF_COMPONENT_TYPE_INT:
      return Data_Type::Int;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
      return Data_Type::UInt;
    case TINYGLTF_COMPONENT_TYPE_FLOAT:
      return Data_Type::Float;
    case TINYGLTF_COMPONENT_TYPE_DOUBLE:
      return Data_Type::Double;
    default:
      REDC_UNREACHABLE_MSG("Unknown component type");
      // This should never be reached.
      return Data_Type::Float;
    }
  }
  Buffer_Target to_buffer_target(int val)
  {
    switch(val)
    {
    case TINYGLTF_TARGET_ARRAY_BUFFER:
      return Buffer_Target::Array;
    case TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER:
      return Buffer_Target::Element_Array;
    default:
      REDC_UNREACHABLE_MSG("Unknown buffer target");
      // This should never be reached.
      return Buffer_Target::Array;
    }
  }

  Attrib_Type to_attrib_type(int val)
  {
    switch(val)
    {
    case TINYGLTF_TYPE_SCALAR:
      return Attrib_Type::Scalar;
    case TINYGLTF_TYPE_VEC2:
      return Attrib_Type::Vec2;
    case TINYGLTF_TYPE_VEC3:
      return Attrib_Type::Vec3;
    case TINYGLTF_TYPE_VEC4:
      return Attrib_Type::Vec4;
    case TINYGLTF_TYPE_MAT2:
      return Attrib_Type::Mat2;
    case TINYGLTF_TYPE_MAT3:
      return Attrib_Type::Mat3;
    case TINYGLTF_TYPE_MAT4:
      return Attrib_Type::Mat4;
    default:
      REDC_UNREACHABLE_MSG("Unknown attribute type");
      // This should never be reached
      return Attrib_Type::Vec4;
    }
  }

  Texture_Target to_texture_target(int val)
  {
    switch(val)
    {
    case TINYGLTF_TEXTURE_TARGET_TEXTURE2D:
      return Texture_Target::Tex_2D;
    default:
      REDC_UNREACHABLE_MSG("Invalid texture target");
      // This should never be reached.
      return Texture_Target::Tex_2D;
    }
  }

  Texture_Format to_texture_format(int val)
  {
    switch(val)
    {
    case TINYGLTF_TEXTURE_FORMAT_ALPHA:
      return Texture_Format::Alpha;
    case TINYGLTF_TEXTURE_FORMAT_RGB:
      return Texture_Format::Rgb;
    case TINYGLTF_TEXTURE_FORMAT_RGBA:
      return Texture_Format::Rgba;
    default:
      REDC_UNREACHABLE_MSG("Invalid / unsupported texture format");
      // This should never be reached.
      return Texture_Format::Rgba;
    };
  }

  Param_Value to_param_value(tinygltf::Parameter const& param, Param_Type type,
                             std::vector<std::string> const& texture_names)
  {
    Param_Value ret;

    switch(type)
    {
    case Param_Type::Byte:
      ret.byte = static_cast<int8_t>(param.number_array[0]);
      break;
    case Param_Type::UByte:
      ret.ubyte = static_cast<uint8_t>(param.number_array[0]);
      break;
    case Param_Type::Short:
      ret.shrt = static_cast<short>(param.number_array[0]);
      break;
    case Param_Type::UShort:
      ret.ushrt = static_cast<unsigned short>(param.number_array[0]);
      break;
    case Param_Type::UInt:
      ret.uint = static_cast<unsigned int>(param.number_array[0]);
      break;
    case Param_Type::IVec4:
      ret.ints[3] = static_cast<int>(param.number_array[3]);
    case Param_Type::IVec3:
      ret.ints[2] = static_cast<int>(param.number_array[2]);
    case Param_Type::IVec2:
      ret.ints[1] = static_cast<int>(param.number_array[1]);
    case Param_Type::Int:
      ret.ints[0] = static_cast<int>(param.number_array[0]);
      break;
    case Param_Type::Vec4:
      ret.floats[3] = static_cast<float>(param.number_array[3]);
    case Param_Type::Vec3:
      ret.floats[2] = static_cast<float>(param.number_array[2]);
    case Param_Type::Vec2:
      ret.floats[1] = static_cast<float>(param.number_array[1]);
    case Param_Type::Float:
      ret.floats[0] = static_cast<float>(param.number_array[0]);
      break;
    case Param_Type::BVec4:
      ret.bools[3] = static_cast<bool>(param.number_array[3]);
    case Param_Type::BVec3:
      ret.bools[2] = static_cast<bool>(param.number_array[2]);
    case Param_Type::BVec2:
      ret.bools[1] = static_cast<bool>(param.number_array[1]);
    case Param_Type::Bool:
      ret.bools[0] = static_cast<bool>(param.number_array[0]);
      break;
    case Param_Type::Mat2:
    case Param_Type::Mat3:
    case Param_Type::Mat4:
      // Copy into our floats array
      std::copy(param.number_array.begin(), param.number_array.end(),
                ret.floats.begin());
      break;
    case Param_Type::Sampler2D:
      REDC_ASSERT(param.string_value.size() != 0);
      // Using the string, find a reference to that texture and store it in the
      // uint member.
      ret.uint = find_string_index(texture_names, param.string_value,
                                   "Parameter value references invalid texture");
      break;
    }

    return ret;
  }

  Render_Mode to_render_mode(int val)
  {
    switch(val)
    {
    case TINYGLTF_MODE_POINTS:
      return Render_Mode::Points;
    case TINYGLTF_MODE_LINE:
      return Render_Mode::Lines;
    case TINYGLTF_MODE_LINE_LOOP:
      return Render_Mode::Line_Loop;
    case TINYGLTF_MODE_TRIANGLES:
      return Render_Mode::Triangles;
    case TINYGLTF_MODE_TRIANGLE_STRIP:
      return Render_Mode::Triangle_Strip;
    case TINYGLTF_MODE_TRIANGLE_FAN:
      return Render_Mode::Triangle_Fan;
    default:
      REDC_UNREACHABLE_MSG("Invalid primitive type");
      // This should never be reached.
      return Render_Mode::Triangles;
    }
  }

  Attrib_Semantic to_attrib_semantic(std::string str, bool recurse = false)
  {
    // Check for an underscore to separate semantic from number
    // ie: SAMPLER_2

    Attrib_Semantic ret;

    auto sep_find = std::find(str.begin(), str.end(), '_');
    if(sep_find == str.end() || recurse)
    {
      // No separator, try to load the string as-is.
      if(str == "POSITION") ret.kind = Attrib_Semantic::Position;
      else if(str == "NORMAL") ret.kind = Attrib_Semantic::Normal;
      else if(str == "TEXCOORD") ret.kind = Attrib_Semantic::Texcoord;
      else if(str == "COLOR") ret.kind = Attrib_Semantic::Color;
      else if(str == "JOINT") ret.kind = Attrib_Semantic::Joint;
      else if(str == "WEIGHT") ret.kind = Attrib_Semantic::Weight;
      else REDC_UNREACHABLE_MSG("Invalid attribute semantic");

      // Don't set this if it wasn't specified initially
      //ret.index = 0;

      return ret;
    }
    else
    {
      auto offset = sep_find - str.begin();

      // Recurse into this function just to parse the semantic string. This
      // can't recurse infinitely and I only did it this way because the type of
      // Attrib_Semantic.kind is an anonymous enum so we can't return it's value
      // directly. I suppose we could with some sort of decltype or something
      // but this is neater.
      auto ret = to_attrib_semantic(str.substr(0, offset), true);
      // Add one to the offset to account for the underscore.
      ret.index = std::stoi(str.substr(offset+1, str.size()));
      return ret;
    }
  }

  Param_Semantic to_param_semantic(std::string str)
  {
    if(str == "LOCAL") return Param_Semantic::Local;
    if(str == "MODEL") return Param_Semantic::Model;
    if(str == "PROJECTION") return Param_Semantic::Projection;
    if(str == "MODELVIEW") return Param_Semantic::Model_View;
    if(str == "MODELVIEWPROJECTION")
      return Param_Semantic::Model_View_Projection;
    if(str == "MODELINVERSE") return Param_Semantic::Model_Inverse;
    if(str == "VIEWINVERSE") return Param_Semantic::View_Inverse;
    if(str == "PROJECTIONINVERSE") return Param_Semantic::Projection_Inverse;
    if(str == "MODELVIEWINVERSE") return Param_Semantic::Model_View_Inverse;
    if(str == "MODELVIEWPROJECTIONINVERSE")
      return Param_Semantic::Model_View_Projection_Inverse;
    if(str == "MODELINVERSETRANSPOSE")
      return Param_Semantic::Model_Inverse_Transpose;
    if(str == "MODELVIEWINVERSETRANSPOSE")
      return Param_Semantic::Model_View_Inverse_Transpose;
    if(str == "VIEWPORT") return Param_Semantic::Viewport;
    if(str == "JOINTMATRIX") return Param_Semantic::Joint_Matrix;

    REDC_UNREACHABLE_MSG("Invalid parameter semantic");
  }

  Param_Type to_param_type(int ty)
  {
    switch(ty)
    {
    case TINYGLTF_PARAMETER_TYPE_BYTE:
      return Param_Type::Byte;
    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
      return Param_Type::UByte;
    case TINYGLTF_PARAMETER_TYPE_SHORT:
      return Param_Type::Short;
    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
      return Param_Type::UShort;
    case TINYGLTF_PARAMETER_TYPE_INT:
      return Param_Type::Int;
    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
      return Param_Type::UInt;
    case TINYGLTF_PARAMETER_TYPE_FLOAT:
      return Param_Type::Float;
    case TINYGLTF_PARAMETER_TYPE_FLOAT_VEC2:
      return Param_Type::Vec2;
    case TINYGLTF_PARAMETER_TYPE_FLOAT_VEC3:
      return Param_Type::Vec3;
    case TINYGLTF_PARAMETER_TYPE_FLOAT_VEC4:
      return Param_Type::Vec4;
    case TINYGLTF_PARAMETER_TYPE_INT_VEC2:
      return Param_Type::IVec2;
    case TINYGLTF_PARAMETER_TYPE_INT_VEC3:
      return Param_Type::IVec3;
    case TINYGLTF_PARAMETER_TYPE_INT_VEC4:
      return Param_Type::IVec4;
    case TINYGLTF_PARAMETER_TYPE_BOOL:
      return Param_Type::Bool;
    case TINYGLTF_PARAMETER_TYPE_BOOL_VEC2:
      return Param_Type::BVec2;
    case TINYGLTF_PARAMETER_TYPE_BOOL_VEC3:
      return Param_Type::BVec3;
    case TINYGLTF_PARAMETER_TYPE_BOOL_VEC4:
      return Param_Type::BVec4;
    case TINYGLTF_PARAMETER_TYPE_FLOAT_MAT2:
      return Param_Type::Mat2;
    case TINYGLTF_PARAMETER_TYPE_FLOAT_MAT3:
      return Param_Type::Mat3;
    case TINYGLTF_PARAMETER_TYPE_FLOAT_MAT4:
      return Param_Type::Mat4;
    case TINYGLTF_PARAMETER_TYPE_SAMPLER_2D:
      return Param_Type::Sampler2D;
    default:
      REDC_UNREACHABLE_MSG("Invalid technique parameter type");
      break;
    }
  }

  // = Destruction

  Asset::~Asset()
  {
    for(auto program : programs)
    {
      destroy_program(program.repr);
    }
    for(auto shader : shaders)
    {
      destroy_shader(shader.repr);
    }
    for(auto& mesh : meshes)
    {
      destroy_meshes(1, &mesh.repr);
    }
    destroy_bufs(buffers.size(), &buffers[0]);
    destroy_textures(textures.size(), &textures[0]);
  }

  // = Load functions

  void load_buffers(tinygltf::Scene const& scene, std::vector<Buf>& bufs,
                    std::vector<std::string>& buf_view_names)
  {
    // Upload all buffer views as GPU buffers
    auto new_buf_reprs = make_buffers(scene.bufferViews.size());

    // Starting index
    std::size_t i = bufs.size();
    bufs.insert(bufs.end(), new_buf_reprs.begin(), new_buf_reprs.end());

    buf_view_names.reserve(buf_view_names.size() + scene.bufferViews.size());

    for(auto pair : scene.bufferViews)
    {
      auto& buf_view = pair.second;

      // Push the name for later cross-referencing, the index of the name in
      // this vector is the same index of the buffer in the scene.
      buf_view_names.push_back(pair.first);

      // Look up the buffer source
      auto buf = scene.buffers.find(buf_view.buffer);
      REDC_ASSERT(buf != scene.buffers.end());

      // Find buffer target
      Buffer_Target buf_target = to_buffer_target(buf_view.target);

      // Upload data to a gpu buffer.
      upload_data(bufs[i], buf->second.data, buf_view.byteOffset,
                  buf_view.byteLength, buf_target);

      ++i;
    }
  }

  void load_nodes_given_names(tinygltf::Scene const& scene,
                              std::vector<std::string> const& node_names,
                              std::vector<Node>& nodes,
                              std::vector<std::string> const& mesh_names)
  {
    nodes.reserve(nodes.size() + node_names.size());

    // This maps names to indexes into the nodes array parameter.
    std::unordered_map<std::string, std::size_t> node_indices;
    for(std::string const& name : node_names)
    {
      auto node_find = scene.nodes.find(name);
      REDC_ASSERT(node_find != scene.nodes.end());
      auto const& in_node = node_find->second;

      Node node;

      // Load in local transformation
      if(in_node.rotation.size())
      {
        node.rotation = std::array<float, 4>{};
        std::copy(in_node.rotation.begin(), in_node.rotation.end(),
                  node.rotation->begin());
      }
      if(in_node.scale.size())
      {
        node.scale = std::array<float, 3>{};
        std::copy(in_node.scale.begin(), in_node.scale.end(),
                  node.scale->begin());
      }
      if(in_node.translation.size())
      {
        node.translation = std::array<float, 3>{};
        std::copy(in_node.translation.begin(),
                  in_node.translation.end(),
                  node.translation->begin());
      }
      if(in_node.matrix.size())
      {
        node.matrix = std::array<float, 16>{};
        std::copy(in_node.matrix.begin(), in_node.matrix.end(),
                  node.matrix->begin());
      }

      // Add any references to meshes
      for(auto req_mesh : in_node.meshes)
      {
        auto mesh_find = std::find(mesh_names.begin(), mesh_names.end(),
                                   req_mesh);
        REDC_ASSERT_MSG(mesh_find != mesh_names.end(),
                        "Node references invalid mesh '%'", req_mesh);
        node.meshes.push_back(mesh_find - mesh_names.begin());
      }

      // Add this node to the main vector of nodes
      nodes.push_back(node);
      // Add its index for later cross-referencing.
      node_indices.insert({name, nodes.size() - 1});
    }

    // Now traverse the hiearchy.
    for(auto const& node_pair : scene.nodes)
    {
      // Tinygltfloader uses a flat list of nodes as well, well it doesn't use a
      // vector but there is no specific hierarchy. Here we are effectively
      // iterating over every node in the asset.

      // For this second pass, add references for each node to its parents and
      // children.
      std::string const& name = node_pair.first;

      // We recorded at what index we put each node in our own scene
      // representation by name, so look it up!
      auto const& node_index_iter = node_indices.find(name);
      REDC_ASSERT(node_index_iter != node_indices.end());

      // Now take that node
      Node& node = nodes[node_index_iter->second];

      // 1. Add indices of our children to this node
      // 2. Add the index of this node to our children.
      for(std::string const& child_name : node_pair.second.children)
      {
        // Find the index of the child
        auto const& child_index_iter = node_indices.find(child_name);
        REDC_ASSERT(child_index_iter != node_indices.end());

        std::size_t child_index = child_index_iter->second;

        // That is one of our children.
        node.children.push_back(child_index);

        // And we are that node's parent.
        nodes[child_index].parent = node_index_iter->second;
      }
    }
  }

  void load_accessors(tinygltf::Scene const& scene,
                      std::vector<Accessor>& accessors,
                      std::vector<std::string>& accessor_names,
                      std::vector<std::string> const& buf_names)
  {
    accessors.reserve(accessors.size() + scene.accessors.size());
    accessor_names.reserve(accessor_names.size() + scene.accessors.size());

    std::size_t i = 0;
    for(auto pair : scene.accessors)
    {
      auto& in_accessor = pair.second;

      // Push the string name for cross-referencing
      accessor_names.push_back(pair.first);

      // Make our accessor
      Accessor acc;

      // Just copy these fields.
      acc.count = in_accessor.count;
      acc.offset = in_accessor.byteOffset;
      acc.stride = in_accessor.byteStride;

      // Find the buffer by name
      acc.buf_i = find_string_index(buf_names, in_accessor.bufferView,
                                    "Accessor references invalid bufferView");

      // Now do data and attribute type
      acc.data_type = to_data_type(in_accessor.componentType);
      acc.attrib_type = to_attrib_type(in_accessor.type);

      accessors.push_back(acc);

      ++i;
    }
  }

  void load_textures(tinygltf::Scene const& scene,
                     std::vector<Texture_Repr>& textures,
                     std::vector<std::string>& texture_names)
  {
    auto new_texture_reprs = make_textures(scene.textures.size());

    // Starting index
    std::size_t i = textures.size();
    textures.insert(textures.end(), new_texture_reprs.begin(),
                    new_texture_reprs.end());

    texture_names.reserve(texture_names.size() + scene.textures.size());

    // We need this because we may be dealing with non-power-of-two RGB textures.
    set_pixel_store_unpack_alignment(1);

    for(auto tex_pair : scene.textures)
    {
      auto& in_tex = tex_pair.second;
      texture_names.push_back(tex_pair.first);

      // Find image by name
      auto image_find = scene.images.find(in_tex.source);
      REDC_ASSERT(image_find != scene.images.end());

      // Load as enums
      Texture_Target target = to_texture_target(in_tex.target);
      Texture_Format format = to_texture_format(in_tex.format);
      Data_Type   data_type = to_data_type(in_tex.type);

      switch(image_find->second.component)
      {
      case 1:
        if(format != Texture_Format::Alpha)
        {
          log_w("Ignoring texture format because image has one component");
          format = Texture_Format::Alpha;
        }
        break;
      case 3:
        if(format != Texture_Format::Rgb)
        {
          log_w("Ignoring texture format because image has three components");
          format = Texture_Format::Rgb;
        }
        break;
      case 4:
        if(format != Texture_Format::Rgba)
        {
          log_w("Ignoring texture format because image has four components");
          format = Texture_Format::Rgba;
        }
        break;
      default:
        REDC_UNREACHABLE_MSG("Unsupported number of image components");
        break;
      }

      // Upload the image data
      upload_image(textures[i], image_find->second.image, target, format,
                   image_find->second.width, image_find->second.height,
                   data_type);

      // tinygltfloader doesn't load in samplers so we'll just guess and
      // generate mipmaps anyway.
      set_sampler(textures[i], target);

      ++i;
    }
  }

  void load_materials(tinygltf::Scene const& scene,
                      std::vector<Material>& materials,
                      std::vector<std::string>& material_names,
                      std::vector<Technique> const& techs,
                      std::vector<std::string> const& tech_names,
                      std::vector<std::string> const& texture_names)
  {
    materials.reserve(materials.size() + scene.materials.size());
    material_names.reserve(material_names.size() + scene.materials.size());

    for(auto mat_pair : scene.materials)
    {
      auto& in_mat = mat_pair.second;

      material_names.push_back(mat_pair.first);

      Material mat;

      // Look up based on the name.
      mat.technique_i = find_string_index(tech_names, in_mat.technique,
                        "Material references invalid semantic");

      auto& technique = techs[mat.technique_i];

      // Now use the technique to find parameter bind points
      for(auto param_pair : in_mat.values)
      {
        // This the name of the technique parameter.
        auto& name = param_pair.first;

        // Find the type of that parameter using the string name
        auto param_decl_find = technique.parameters.find(name);
        REDC_ASSERT(param_decl_find != technique.parameters.end());

        // Get the value of this parameter
        Parameter param;
        param.type = param_decl_find->second.type;
        param.value = to_param_value(param_pair.second, param.type,
                                     texture_names);

        // Get the technique parameter from the name.
        auto tech_param_find = technique.parameters.find(name);
        REDC_ASSERT(tech_param_find != technique.parameters.end());
        REDC_ASSERT_MSG(tech_param_find->second.bind.which() == 0, "Material "
                        "value must reference a parameter, not an attribute");

        // Find the bind point for this parameter.
        Param_Bind bind =
          boost::get<Param_Bind>(tech_param_find->second.bind);

        mat.parameters.emplace_back(bind, param);
      }

      materials.push_back(std::move(mat));
    }
  }

  void load_meshes(tinygltf::Scene const& scene, std::vector<Mesh>& meshes,
                   std::vector<std::string>& mesh_names,
                   std::vector<std::string> const& accessor_names,
                   std::vector<std::string> const& mat_names)
  {
    meshes.reserve(meshes.size() + scene.meshes.size());
    mesh_names.reserve(mesh_names.size() + scene.meshes.size());

    // Make one representation for each mesh
    auto mesh_reprs = make_mesh_reprs(scene.meshes.size());

    std::size_t repr_i = 0;
    for(auto mesh_pair : scene.meshes)
    {
      mesh_names.push_back(mesh_pair.first);

      Mesh our_mesh;

      // Give our mesh one of the created representations.
      our_mesh.repr = mesh_reprs[repr_i++];

      for(auto& in_prim : mesh_pair.second.primitives)
      {
        Primitive prim;

        prim.mode = to_render_mode(in_prim.mode);

        // Find the referenced material
        prim.mat_i =
          find_string_index(mat_names, in_prim.material,
                            "Primitive references invalid material name");

        // Add references to any accessors and their semantics.
        for(auto accessor_pair : in_prim.attributes)
        {
          auto name = accessor_pair.first;
          auto accessor = accessor_pair.second;

          // String => Semantic
          auto semantic = to_attrib_semantic(name);

          // Find the index / ref of the accessor
          auto access_ref =
            find_string_index(accessor_names, accessor,
                              "Primitive references invalid accessor");

          // We know have an accessor bound to some attribute point.
          prim.attributes.emplace(semantic, access_ref);
        }

        // Enable enough vertex attributes in the mesh repr to hold every
        // attribute in the primitive. With this implementation we are assuming
        // that they are contiguous and start at 0.
        enable_vertex_attrib_arrays(our_mesh.repr, 0,in_prim.attributes.size());

        // Indices attribute
        if(!in_prim.indices.empty())
        {
          // Use this accessor for the element arrayxy
          prim.indices =
            find_string_index(accessor_names, in_prim.indices,
                              "Primitive references invalid accessor");
        }

        our_mesh.primitives.push_back(prim);
      }
      meshes.push_back(std::move(our_mesh));
    }
  }

  void load_shaders(tinygltf::Scene const& scene,
                    std::vector<Shader>& shaders,
                    std::vector<std::string>& shader_names)
  {
    shaders.reserve(shaders.size() + scene.shaders.size());
    shader_names.reserve(shader_names.size() + scene.shaders.size());

    for(auto shader_pair : scene.shaders)
    {
      tinygltf::Shader const& in_shader = shader_pair.second;

      Shader_Type shader_type;
      switch(in_shader.type)
      {
      case TINYGLTF_SHADER_TYPE_VERTEX_SHADER:
        shader_type = Shader_Type::Vertex;
        break;
      case TINYGLTF_SHADER_TYPE_FRAGMENT_SHADER:
        shader_type = Shader_Type::Fragment;
        break;
      }

      // Make the shader (representation)
      auto shader = make_shader(shader_type);

      // Upload source
      upload_shader_source(shader, (char*) &in_shader.source[0],
                           in_shader.source.size());

      // Now compile the shader and handle errors, if there are errors refuse to
      // add the shader, log and continue to the next shader
      std::vector<char> info_log;
      if(compile_shader(shader, &info_log))
      {
        if(info_log.size() > 0)
        {
          std::string log(info_log.begin(), info_log.end());
          log_i("Shader '%' compiled successfully with info log:\n%",
                shader_pair.first, log);
        }
        shader_names.push_back(shader_pair.first);
        shaders.push_back(shader);
      }
      else
      {
        std::string log(info_log.begin(), info_log.end());
        log_e("Failed to compile shader '%':\n%", shader_pair.first, log);
        continue;
      }
    }
  }

  void load_programs(tinygltf::Scene const& scene,
                     std::vector<Program>& programs,
                     std::vector<std::string>& program_names,
                     std::vector<Shader> const& shaders,
                     std::vector<std::string> const& shader_names)
  {
    programs.reserve(programs.size() + scene.programs.size());
    program_names.reserve(program_names.size() + scene.programs.size());

    for(auto const& program_pair : scene.programs)
    {
      tinygltf::Program const& in_program = program_pair.second;

      // Make a program repr
      Program program;
      program.repr = make_program();

      // Find vertex shader
      auto vert_i = find_string_index(shader_names, in_program.vertexShader,
                    "Program references invalid vertex shader");
      // Find the shader repr and attach that to our program repr
      attach_shader(program.repr, shaders[vert_i].repr);

      auto frag_i = find_string_index(shader_names, in_program.fragmentShader,
                    "Program references invalid fragment shader");
      attach_shader(program.repr, shaders[frag_i].repr);

      std::vector<char> link_log;
      if(link_program(program.repr, &link_log))
      {
        if(link_log.size())
        {
          std::string log(link_log.begin(), link_log.end());
          log_i("Information while linking program '%':\n%",
                program_pair.first, log);
        }

        // Link attribute names to binds.
        for(auto attribute_name : in_program.attributes)
        {
          // Find the bind and add it to the program's list of attributes
          program.attributes.emplace(attribute_name,
                                get_attrib_bind(program.repr, attribute_name));
        }

        // Add this program to our list
        programs.push_back(program);
        program_names.push_back(program_pair.first);
      }
      else
      {
        std::string log(link_log.begin(), link_log.end());
        log_e("Failed to link program '%':\n%",
              program_pair.first, log);
      }
    }
  }

  void load_techniques(tinygltf::Scene const& scene,
                       std::vector<Technique>& techniques,
                       std::vector<std::string>& technique_names,
                       std::vector<Program> const& programs,
                       std::vector<std::string> const& program_names,
                       std::vector<std::string> const& texture_names,
                       std::vector<std::string> const& node_names)
  {
    techniques.reserve(techniques.size() + scene.techniques.size());
    technique_names.reserve(technique_names.size() + scene.techniques.size());

    for(auto const& technique_pair : scene.techniques)
    {
      tinygltf::Technique const& in_technique = technique_pair.second;

      technique_names.push_back(technique_pair.first);

      Technique technique;

      // Find program of this technique
      auto program_i = find_string_index(program_names, in_technique.program,
                       "Technique references invalid program");
      technique.program_i = program_i;

      Program const& program = programs[program_i];

      std::unordered_map<std::string, Attrib_Bind> attributes;
      for(auto const& attrib_pair : in_technique.attributes)
      {
        // We already have the bind in the program struct, we just need to find
        // it given the string, which is the key (first element) of this
        // dictionary. The value (second element) is what we are interested in,
        // so it will be the key of this new attributes map.

        auto find_bind = program.attributes.find(attrib_pair.first);
        REDC_ASSERT_MSG(find_bind != program.attributes.end(),
                        "Technique references invalid attribute");
        attributes.emplace(attrib_pair.second, find_bind->second);
      }

      std::unordered_map<std::string, Param_Bind> uniforms;
      for(auto const& uniform_pair : in_technique.uniforms)
      {
        // Uniform identifier
        auto const& uniform_ident = uniform_pair.second;
        // Uniform name, this is the name in the GLSL source code
        auto const& uniform_name = uniform_pair.first;

        // Get the bind of the uniform
        auto bind = get_param_bind(program.repr, uniform_name);

        uniforms.emplace(uniform_ident, bind);
      }

      for(auto const& param_pair : in_technique.parameters)
      {
        tinygltf::TechniqueParameter const& in_param = param_pair.second;

        Param_Decl param;

        param.count = in_param.count;

        if(in_param.node.size())
        {
          auto node_ref = find_string_index(node_names, in_param.node,
                          "Technique parameter references invalid node");
          param.node = node_ref;
        }

        param.type = to_param_type(in_param.type);

        // Is this parameter an attribute?
        if(attributes.count(param_pair.first))
        {
          // Does it have a semantic?
          if(in_param.semantic.size())
          {
            param.semantic = to_attrib_semantic(in_param.semantic);
          }

          // We already know the attribute bind
          param.bind = attributes[param_pair.first];
        }
        else
        {
          // Does it have a semantic?
          if(in_param.semantic.size())
          {
            param.semantic = to_param_semantic(in_param.semantic);
          }

          // We already know the parameter bind
          param.bind = uniforms[param_pair.first];
        }

        if(in_param.value.string_value.size() == 0 &&
           in_param.value.number_array.size() == 0)
        {
          param.default_value = boost::none;
        }
        else
        {
          param.default_value = to_param_value(in_param.value, param.type,
                                               texture_names);
        }

        technique.parameters.emplace(param_pair.first, std::move(param));
      }

      techniques.push_back(std::move(technique));
    }
  }

  void load_node_names(tinygltf::Scene const& scene,
                       std::vector<std::string>& node_names)
  {
    for(auto node_pair : scene.nodes)
    {
      node_names.push_back(node_pair.first);
    }
  }

  Asset load_asset(tinygltf::Scene const& scene)
  {
    Asset ret;

    std::vector<std::string> buf_view_names;
    load_buffers(scene, ret.buffers, buf_view_names);

    std::vector<std::string> accessor_names;
    load_accessors(scene, ret.accessors, accessor_names, buf_view_names);

    std::vector<std::string> texture_names;
    load_textures(scene, ret.textures, texture_names);

    // Load shaders and programs

    std::vector<std::string> shader_names;
    load_shaders(scene, ret.shaders, shader_names);

    std::vector<std::string> program_names;
    load_programs(scene, ret.programs, program_names, ret.shaders,shader_names);

    std::vector<std::string> node_names;
    load_node_names(scene, node_names);

    std::vector<std::string> technique_names;
    load_techniques(scene, ret.techniques, technique_names, ret.programs,
                    program_names, texture_names, node_names);

    std::vector<std::string> material_names;
    load_materials(scene, ret.materials, material_names, ret.techniques,
                   technique_names, texture_names);

    std::vector<std::string> mesh_names;
    load_meshes(scene, ret.meshes, mesh_names, accessor_names, material_names);

    // Load nodes
    load_nodes_given_names(scene, node_names, ret.nodes, mesh_names);
    return ret;
  }

  glm::mat4 local_transformation(Node const& node)
  {
      // Find the model of the current node and then go up a level
      glm::mat4 this_model(1.0f);

      // Replace the identity matrix above with the optional matrix in the node
      if(node.matrix)
      {
        std::memcpy(&this_model[0], &node.matrix.value()[0], 16 * sizeof(float));
      }

      // Scale, rotate and then translate! I don't believe having both a matrix
      // and scale/rotation/translation is allowed in the standard so we
      // shouldn't worry about the order of these two values, just as long as
      // these three are properly ordered.
      if(node.scale)
      {
        auto& arr = node.scale.value();
        this_model = glm::scale(this_model, glm::vec3(arr[0], arr[1], arr[2]));
      }
      if(node.rotation)
      {
        auto& arr = node.rotation.value();
        glm::quat rot(arr[0], arr[1], arr[2], arr[3]);
        this_model = mat4_cast(rot) * this_model;
      }
      if(node.translation)
      {
        auto& arr = node.translation.value();
        this_model = glm::translate(this_model, glm::vec3(arr[0], arr[1], arr[2]));
      }

      return this_model;
  }
  glm::mat4 model_transformation(std::vector<Node> const& nodes, Node_Ref child)
  {
    boost::optional<Node_Ref> cur_node = child;
    glm::mat4 cur_model(1.0f);

    while(cur_node)
    {
      Node const& node = nodes[cur_node.value()];

      cur_model = cur_model * local_transformation(node);
      cur_node = node.parent;
    }

    return cur_model;
  }

  // TODO: This is a bad name
  struct Render_Params
  {
    Mesh_Repr mesh_repr;
    Primitive const* primitive;
    glm::mat4 local;
    glm::mat4 model;
  };

  glm::mat4 get_local(Param_Decl param, Asset const& asset,
                      Render_Params const& cur_render)
  {
    // If the semantic mentions a particular node, use its model, otherwise use
    // the model from the render params
    if(param.node)
      return local_transformation(asset.nodes[param.node.value()]);

    return cur_render.local;
  }
  glm::mat4 get_model(Param_Decl param, Asset const& asset,
                      Render_Params const& cur_render)
  {
    // If the semantic mentions a particular node, use its model, otherwise use
    // the model from the render params
    if(param.node)
      return model_transformation(asset.nodes, param.node.value());

    return cur_render.model;
  }
  glm::mat4 get_view(gfx::Camera const& cam)
  {
    return camera_view_matrix(cam);
  }
  glm::mat4 get_proj(gfx::Camera const& cam)
  {
    return camera_proj_matrix(cam);
  }

  // This should really be two functions one to retrieve / calculate the value
  // of the semantic and one to set it but it's hard to do this efficiently
  // because of the copying between matrices / arrays.
  Param_Value set_semantic_value(Param_Decl param,
                                 Asset const& asset,
                                 Render_Params const& cur_render,
                                 gfx::Camera const& cam)
  {
    // We must be dealing with semantic parameters
    REDC_ASSERT(static_cast<bool>(param.semantic) == true);
    REDC_ASSERT(param.semantic.value().which() == 0);
    REDC_ASSERT(param.bind.which() == 0);

    Param_Semantic semantic =
      boost::get<Param_Semantic>(param.semantic.value());

    Param_Bind bind = boost::get<Param_Bind>(param.bind);

    switch(semantic)
    {
    case Param_Semantic::Local:
    {
      REDC_ASSERT(param.type == Param_Type::Mat4);
      glm::mat4 model = get_local(param, asset, cur_render);
      set_mat4_parameter(bind, glm::value_ptr(model));
      break;
    }
    case Param_Semantic::Model:
    {
      REDC_ASSERT(param.type == Param_Type::Mat4);
      glm::mat4 model = get_model(param, asset, cur_render);
      set_mat4_parameter(bind, glm::value_ptr(model));
      break;
    }
    case Param_Semantic::View:
    {
      REDC_ASSERT(param.type == Param_Type::Mat4);
      glm::mat4 view = get_view(cam);
      set_mat4_parameter(bind, glm::value_ptr(view));
      break;
    }
    case Param_Semantic::Projection:
    {
      REDC_ASSERT(param.type == Param_Type::Mat4);
      glm::mat4 proj = camera_proj_matrix(cam);
      set_mat4_parameter(bind, glm::value_ptr(proj));
      break;
    }
    case Param_Semantic::Model_View:
    {
      REDC_ASSERT(param.type == Param_Type::Mat4);
      glm::mat4 model = get_model(param, asset, cur_render);
      glm::mat4 view = get_view(cam);
      glm::mat4 view_model = view * model;
      set_mat4_parameter(bind, glm::value_ptr(view_model));
      break;
    }
    case Param_Semantic::Model_View_Projection:
    {
      REDC_ASSERT(param.type == Param_Type::Mat4);
      glm::mat4 model = get_model(param, asset, cur_render);
      glm::mat4 view = get_view(cam);
      glm::mat4 proj = get_proj(cam);
      glm::mat4 mat = proj * view * model;
      set_mat4_parameter(bind, glm::value_ptr(mat));
      break;
    }
    case Param_Semantic::Model_Inverse:
    {
      glm::mat4 model = get_model(param, asset, cur_render);
      glm::mat4 mat = glm::inverse(model);
      set_mat4_parameter(bind, glm::value_ptr(mat));
      break;
    }
    case Param_Semantic::View_Inverse:
    {
      glm::mat4 view = get_view(cam);
      glm::mat4 mat = glm::inverse(view);
      set_mat4_parameter(bind, glm::value_ptr(mat));
      break;
    }
    case Param_Semantic::Projection_Inverse:
    {
      glm::mat4 proj = get_proj(cam);
      glm::mat4 mat = glm::inverse(proj);
      set_mat4_parameter(bind, glm::value_ptr(mat));
      break;
    }
    case Param_Semantic::Model_View_Inverse:
    {
      REDC_ASSERT(param.type == Param_Type::Mat4);
      glm::mat4 model = get_model(param, asset, cur_render);
      glm::mat4 view = get_view(cam);
      glm::mat4 view_model = glm::inverse(view * model);
      set_mat4_parameter(bind, glm::value_ptr(view_model));
      break;
    }
    case Param_Semantic::Model_View_Projection_Inverse:
    {
      REDC_ASSERT(param.type == Param_Type::Mat4);
      glm::mat4 model = get_model(param, asset, cur_render);
      glm::mat4 view = get_view(cam);
      glm::mat4 proj = get_proj(cam);
      glm::mat4 mat = glm::inverse(proj * view * model);
      set_mat4_parameter(bind, glm::value_ptr(mat));
      break;
    }
    case Param_Semantic::Model_Inverse_Transpose:
    {
      glm::mat4 model = get_model(param, asset, cur_render);
      glm::mat4 mat = glm::transpose(glm::inverse(model));
      set_mat4_parameter(bind, glm::value_ptr(mat));
      break;
    }
    case Param_Semantic::Model_View_Inverse_Transpose:
    {
      REDC_ASSERT(param.type == Param_Type::Mat3);
      glm::mat4 model = get_model(param, asset, cur_render);
      glm::mat4 view = get_view(cam);
      glm::mat3 mat = glm::mat3(glm::transpose(glm::inverse(view * model)));
      set_mat3_parameter(bind, glm::value_ptr(mat));
      break;
    }
    default:
      REDC_UNREACHABLE_MSG("Rendering code doesn't support this param "
                           "semantic (%)", static_cast<unsigned int>(semantic));
      break;
    }
  }

  void render_asset(Asset const& asset, gfx::Camera const& camera,
                    Rendering_State& cur_rendering_state)
  {
    // Figure out all the models of every mesh.
    // TODO: This can be cached!
    std::vector<Render_Params> render_params;

    std::size_t node_i = 0;
    for(auto node : asset.nodes)
    {
      glm::mat4 model = model_transformation(asset.nodes, node_i);
      glm::mat4 local_mat = local_transformation(asset.nodes[node_i]);

      // If there is a mesh associated with that node we need to render it
      for(std::size_t mesh_ref : node.meshes)
      {
        Mesh const& mesh = asset.meshes[mesh_ref];
        for(Primitive const& primitive : asset.meshes[mesh_ref].primitives)
        {
          Render_Params render;
          render.mesh_repr = mesh.repr;
          render.primitive = &primitive;
          render.model = model;
          render.local = local_mat;
          render_params.push_back(std::move(render));
        }
      }

      ++node_i;
    }

    // Sort by technique first and material second
    std::sort(render_params.begin(), render_params.end(),
    [&](auto const& lhs, auto const& rhs)
    {
      Primitive const& lhprim = *lhs.primitive;
      Primitive const& rhprim = *rhs.primitive;

      Material const& lhmat = asset.materials[lhprim.mat_i];
      Material const& rhmat = asset.materials[rhprim.mat_i];

      // If the techniques are the same:
      if(lhmat.technique_i == rhmat.technique_i)
      {
        // Sort by material.
        return lhprim.mat_i < rhprim.mat_i;
      }
      // Otherwise sort by technique.
      return lhmat.technique_i < rhmat.technique_i;
    });

    // Render each set of parameters!
    for(auto render : render_params)
    {
      Primitive const& primitive = *render.primitive;

      Material const& mat = asset.materials[primitive.mat_i];
      Technique const& technique = asset.techniques[mat.technique_i];

      // This should be very efficient because of the above sort.
      if(cur_rendering_state.cur_material_i != primitive.mat_i)
      {
        // Load the material of the primitive.

        // Set this state for later.
        cur_rendering_state.cur_material_i = primitive.mat_i;

        if(cur_rendering_state.cur_technique_i != mat.technique_i)
        {
          // Load the technique of the material.

          // Set this state for later.
          cur_rendering_state.cur_technique_i = mat.technique_i;

          // Use the shader program.
          use_program(asset.programs[technique.program_i].repr);

          // Now set each parameter
          for(auto param_pair : technique.parameters)
          {
            // We only care about parameters (uniforms). Attributes will be set
            // later on.
            if(param_pair.second.bind.which() != 0) continue;

            // We don't know how to do count
            REDC_ASSERT_MSG(param_pair.second.count == 1,
                            "technique parameter '%' must have count == 1",
                            param_pair.first);

            Param_Decl const& param = param_pair.second;

            Param_Bind bind = boost::get<Param_Bind>(param.bind);

            // If there isn't a semantic but it has a default value it will be
            // static so it won't need be set again as long as another technique
            // hasn't been used.
            if(!param.semantic && param.default_value)
            {
              // Set the value if it has one
              set_parameter(bind, param.type, param.default_value.value(),
                            asset.textures);
            }
          }
        }

        // Override technique parameters with values in the material
        for(auto parameter_pair : mat.parameters)
        {
          set_parameter(parameter_pair.first, parameter_pair.second,
                        asset.textures);
        }
      }


      // For each semantic technique parameter
      // Is it possible for these to override material specific values? And is
      // that a bug?
      for(auto param_pair : technique.parameters)
      {
        Param_Decl const& param = param_pair.second;

        // We can only do this for parameters that have a semantic
        if(param.bind.which() != 0) continue;
        if(!param.semantic) continue;

        // Retrieve / calculate and set semantic value.
        set_semantic_value(param, asset, render, camera);
      }

      // Format the mesh vao for this set of primitives.
      // TODO: Cache this

      use_mesh(render.mesh_repr);

      std::size_t min_elements = 0;
      for(auto attribute : primitive.attributes)
      {
        Attrib_Semantic semantic = attribute.first;
        Accessor const& accessor = asset.accessors[attribute.second];

        Attrib_Bind bind = get_attrib_semantic_bind(technique, semantic);

        use_array_accessor(bind, asset.buffers[accessor.buf_i], accessor);

        min_elements = std::min(min_elements, accessor.count);
      }
      if(primitive.indices)
      {
        Accessor const& indices = asset.accessors[primitive.indices.value()];
        use_element_array_accessor(asset.buffers[indices.buf_i], indices);

        draw_elements(indices.count, indices.data_type, primitive.mode,
                      indices.offset);
      }
      else
      {
        draw_arrays(min_elements, primitive.mode);
      }
    }
  }
}
