/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "shader.h"

#include <array>
#include <string>
#include "glad/glad.h"

#include "../../common/log.h"
#include "driver.h"

#include "../../common/debugging.h"

// If we are debugging this will cause a crash, and just in case we return
// should the macro be deactivated
#define LOC_BAIL(location) \
  if(location == -1) return

// We used to assert and crash in the above macro like this.
//REDC_ASSERT_MSG(location != -1, "Bad uniform location in shader");

#include <boost/filesystem.hpp>
#define ASSERT_FILE(filepath) \
  REDC_ASSERT_MSG(boost::filesystem::exists(filepath), "Shader file %" \
                  " doesn't exist", filepath)

#define USE_THIS_SHADER() driver_->use_shader(*this)

// TODO @ Update to OpenGL 4.x: Use the new functions for setting uniforms
// that don't require a state change.

namespace redc { namespace gfx { namespace gl
{
  void compile_shader(GLuint shade, GL_Shader::shader_source_t const& source,
                      std::string filename = "")
  {
    // Copy the shader source
    int len = source.size();
    auto data_cstr = source.data();
    glShaderSource(shade, 1, &data_cstr, &len);

    // Compile
    glCompileShader(shade);

    // What are the severity of these messages
    auto severity = Log_Severity::Warning;

    GLint compiled = 0;
    glGetShaderiv(shade, GL_COMPILE_STATUS, &compiled);

    // If the compile failed, they are errors
    if(compiled == GL_FALSE)
    {
      severity = Log_Severity::Error;
      log_e("Shader '%' failed to compile", filename);
    }

    // Check the info log for any messages
    GLint info_log_length = 0;
    glGetShaderiv(shade, GL_INFO_LOG_LENGTH, &info_log_length);
    if(info_log_length != 0)
    {
      std::string info_log(info_log_length, 0x00);
      glGetShaderInfoLog(shade, info_log_length, NULL, &info_log[0]);
      log(severity, "Shader info log of '%' (info log):\n%", filename,
          info_log);
    }
    else
    {
      // By default, the no shader info log message has info severity.
      auto no_info_sev = Log_Severity::Info;
      // But a compile failure makes this important
      if(!compiled) no_info_sev = Log_Severity::Error;
      // Now do the log
      log(no_info_sev, "No shader info log for '%'", filename);
    }
  }
  GL_Shader::GL_Shader(Driver& d) : driver_(&d)
  {
    allocate_shader_();
  }
  GL_Shader::~GL_Shader()
  {
    unallocate_shader_();
  }

  void GL_Shader::allocate_shader_()
  {
    prog_ = glCreateProgram();
  }
  void GL_Shader::unallocate_shader_()
  {
    linked_ = false;
    tags.clear();

    if(prog_) glDeleteProgram(prog_);
    if(g_shade_) glDeleteShader(g_shade_);
    if(f_shade_) glDeleteShader(f_shade_);
    if(v_shade_) glDeleteShader(v_shade_);
  }

  void GL_Shader::reinitialize()
  {
    unallocate_shader_();
    allocate_shader_();
  }

  void GL_Shader::load_part(shader_source_t const& source, std::string name,
                            GLenum part, GLuint& shade_obj)
  {
    // If the shader hasn't been created yet
    if(!shade_obj)
    {
      // Create a new shader
      shade_obj = glCreateShader(part);
    }

    // Attach it to our program
    glAttachShader(prog_, shade_obj);

    // Compile the shader with the given code
    compile_shader(shade_obj, source, name);
  }
  void GL_Shader::load_vertex_part(shader_source_t const& code,
                                   std::string const& name)
  {
    load_part(code, name, GL_VERTEX_SHADER, v_shade_);
  }
  void GL_Shader::load_fragment_part(shader_source_t const& code,
                                     std::string const& name)
  {
    load_part(code, name, GL_FRAGMENT_SHADER, f_shade_);
  }
  void GL_Shader::load_geometry_part(shader_source_t const& code,
                                     std::string const& name)
  {
    load_part(code, name, GL_GEOMETRY_SHADER, g_shade_);
  }

  bool GL_Shader::link()
  {
    // Make sure we have a vertex and fragment shader, otherwise we know it's
    // not going to work.
    REDC_ASSERT_MSG(v_shade_, "OpenGL shader program must have vertex shader");
    REDC_ASSERT_MSG(f_shade_, "OpenGL shader program must have fragment shader");

    // The tags are going to be invalid after a new link. Technically if the
    // shaders haven't changed the tags don't need to be invalidated.
    tags.clear();

    // Attempt the link
    glLinkProgram(prog_);

    // Check the log
    GLint result = 0;
    glGetProgramiv(prog_, GL_LINK_STATUS, &result);
    if(result == GL_FALSE)
    {
      // Link failed.
      GLint info_log_length = 0;
      glGetProgramiv(prog_, GL_INFO_LOG_LENGTH, &info_log_length);

      if(info_log_length == 0)
      {
        log_e("Program link failed (no info log)");
      }
      else
      {
        std::string info_log(info_log_length, 0x00);
        glGetProgramInfoLog(prog_, info_log_length, NULL, &info_log[0]);

        log_e("Program link failed (info log):\n%", info_log);
      }
    }
    else
    {
      linked_ = true;
    }

    return linked_;
  }

  void GL_Shader::set_var_tag(tag_t tag, std::string var_name)
  {
    // The shader must have been linked by now.
    REDC_ASSERT_MSG(linked_, "Adding tags requires a linked program");

    // Get the location, we don't need to switch to our program to do this!
    auto loc = glGetUniformLocation(prog_, var_name.data());
    // If it's not valid don't put it in!
    LOC_BAIL(loc);

    // Commit it, this is basically insert_or_assign but we don't have C++17 yet
    // so this will have to do.
    if(tags.count(tag)) tags.at(tag) = loc;
    else tags.insert({tag, loc});
  }

  // Functions to set uniforms given a bind point
  void GL_Shader::set_vec2(Param_Bind bind, float const* vals)
  {
    USE_THIS_SHADER();
    glUniform2fv(bind, 1, vals);
  }
  void GL_Shader::set_vec2(Param_Bind bind, glm::vec2 const& vec)
  {
    USE_THIS_SHADER();
    glUniform2f(bind, vec.x, vec.y);
  }
  void GL_Shader::set_vec3(Param_Bind bind, float const* vals)
  {
    USE_THIS_SHADER();
    glUniform3fv(bind, 1, vals);
  }
  void GL_Shader::set_vec3(Param_Bind bind, glm::vec3 const& vec)
  {
    USE_THIS_SHADER();
    glUniform3f(bind, vec.x, vec.y, vec.z);
  }
  void GL_Shader::set_vec4(Param_Bind bind, float const* vals)
  {
    USE_THIS_SHADER();
    glUniform4fv(bind, 1, vals);
  }
  void GL_Shader::set_vec4(Param_Bind bind, glm::vec4 const& vec)
  {
    USE_THIS_SHADER();
    glUniform4f(bind, vec.x, vec.y, vec.z, vec.w);
  }
  void GL_Shader::set_ivec2(Param_Bind bind, int const* vals)
  {
    USE_THIS_SHADER();
    glUniform2iv(bind, 1, vals);
  }
  void GL_Shader::set_ivec2(Param_Bind bind, glm::ivec2 const& vec)
  {
    USE_THIS_SHADER();
    glUniform2i(bind, vec.x, vec.y);
  }
  void GL_Shader::set_ivec3(Param_Bind bind, int const* vals)
  {
    USE_THIS_SHADER();
    glUniform3iv(bind, 1, vals);
  }
  void GL_Shader::set_ivec3(Param_Bind bind, glm::ivec3 const& vec)
  {
    USE_THIS_SHADER();
    glUniform3i(bind, vec.x, vec.y, vec.z);
  }
  void GL_Shader::set_ivec4(Param_Bind bind, int const* vals)
  {
    USE_THIS_SHADER();
    glUniform4iv(bind, 1, vals);
  }
  void GL_Shader::set_ivec4(Param_Bind bind, glm::ivec4 const& vec)
  {
    USE_THIS_SHADER();
    glUniform4i(bind, vec.x, vec.y, vec.z, vec.w);
  }
  void GL_Shader::set_bvec2(Param_Bind bind, bool const* vals)
  {
    USE_THIS_SHADER();

    std::array<int, 2> arr;
    std::copy(vals, vals + 2, arr.begin());
    glUniform2iv(bind, 1, &arr[0]);
  }
  void GL_Shader::set_bvec2(Param_Bind bind, glm::bvec2 const& vec)
  {
    USE_THIS_SHADER();
    glUniform2i(bind, vec.x, vec.y);
  }
  void GL_Shader::set_bvec3(Param_Bind bind, bool const* vals)
  {
    USE_THIS_SHADER();

    std::array<int, 3> arr;
    std::copy(vals, vals + 3, arr.begin());
    glUniform3iv(bind, 1, &arr[0]);
  }
  void GL_Shader::set_bvec3(Param_Bind bind, glm::bvec3 const& vec)
  {
    USE_THIS_SHADER();
    glUniform3i(bind, vec.x, vec.y, vec.z);
  }
  void GL_Shader::set_bvec4(Param_Bind bind, bool const* vals)
  {
    USE_THIS_SHADER();

    std::array<int, 4> arr;
    std::copy(vals, vals + 4, arr.begin());
    glUniform4iv(bind, 1, &arr[0]);
  }
  void GL_Shader::set_bvec4(Param_Bind bind, glm::bvec4 const& vec)
  {
    USE_THIS_SHADER();
    glUniform4i(bind, vec.x, vec.y, vec.z, vec.w);
  }
  void GL_Shader::set_mat2(Param_Bind bind, float const* vals)
  {
    USE_THIS_SHADER();
    glUniformMatrix2fv(bind, 1, GL_FALSE, vals);
  }
  void GL_Shader::set_mat3(Param_Bind bind, float const* vals)
  {
    USE_THIS_SHADER();
    glUniformMatrix3fv(bind, 1, GL_FALSE, vals);
  }
  void GL_Shader::set_mat4(Param_Bind bind, float const* vals)
  {
    USE_THIS_SHADER();
    glUniformMatrix4fv(bind, 1, GL_FALSE, vals);
  }
  void GL_Shader::set_float(Param_Bind bind, float val)
  {
    USE_THIS_SHADER();
    glUniform1f(bind, val);
  }
  void GL_Shader::set_integer(Param_Bind bind, int val)
  {
    USE_THIS_SHADER();
    glUniform1i(bind, val);
  }
  void GL_Shader::set_bool(Param_Bind bind, bool val)
  {
    USE_THIS_SHADER();
    glUniform1i(bind, val);
  }

  void GL_Shader::use()
  {
    REDC_ASSERT_MSG(linked_, "Cannot activate a non-linked shader program");
    glUseProgram(prog_);
  }

  GLint GL_Shader::get_location_from_tag(tag_t tag) const
  {
    if(tags.count(tag))
    {
      // If the tag exists return that value from the map
      return tags.at(tag);
    }
    // Otherwise we the tag isn't valid, return a bad location
    return bad_attrib_bind();
  }

  Attrib_Bind GL_Shader::get_attrib_bind(std::string attrib) const
  {
    return glGetAttribLocation(prog_, attrib.c_str());
  }
  Param_Bind GL_Shader::get_param_bind(std::string param) const
  {
    return glGetUniformLocation(prog_, param.c_str());
  }
  Param_Bind GL_Shader::get_tag_param_bind(std::string tag) const
  {
    return (Param_Bind) get_location_from_tag(tag);
  }


} } }

#undef LOC_BAIL
#undef ASSERT_FILE
#undef USE_THIS_SHADER
