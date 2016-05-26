/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "shader.h"

#include <string>
#include "glad/glad.h"

#include <glm/gtc/type_ptr.hpp>

#include "../../common/log.h"
#include "driver.h"

#include "../../common/debugging.h"

// If we are debugging this will cause a crash, and just in case we return
// should the macro be deactivated
#define LOC_BAIL(location) \
  //REDC_ASSERT_MSG(location != -1, "Bad uniform location in shader");  \
  if(location == -1) return

#include <boost/filesystem.hpp>
#define ASSERT_FILE(filepath) \
  REDC_ASSERT_MSG(boost::filesystem::exists(filepath), "Shader file %" \
                  " doesn't exist", filepath)

#define USE_THIS_SHADER() driver_->use_shader(*this)

namespace redc { namespace gfx { namespace gl
{
  void compile_shader(GLuint shade, GL_Shader::shader_source_t const& source,
                      std::string filename = "")
  {
    int len = source.size();
    auto data_cstr = source.data();
    glShaderSource(shade, 1, &data_cstr, &len);

    glCompileShader(shade);

    GLint result = 0;
    glGetShaderiv(shade, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE)
    {
      // Compilation failed.
      GLint info_log_length = 0;
      glGetShaderiv(shade, GL_INFO_LOG_LENGTH, &info_log_length);
      auto info_log = new char[info_log_length];

      glGetShaderInfoLog(shade, info_log_length, NULL, info_log);

      log_e("Shader compilation failed in '%' (info log):\n%", filename,
            info_log);

      delete[] info_log;
    }
  }
  GL_Shader::GL_Shader(Driver& d) : driver_(&d)
  {
    prog_ = glCreateProgram();
  }
  GL_Shader::~GL_Shader()
  {
    if(prog_) glDeleteProgram(prog_);
    if(g_shade_) glDeleteShader(g_shade_);
    if(f_shade_) glDeleteShader(f_shade_);
    if(v_shade_) glDeleteShader(v_shade_);
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
      auto info_log = new char[info_log_length];

      glGetProgramInfoLog(prog_, info_log_length, NULL, info_log);

      log_e("Program link failed (info log):\n%", info_log);

      delete[] info_log;
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

  // Functions to set uniforms given a tag

  void GL_Shader::set_mat4(tag_t tag, glm::mat4 const& mat)
  {
    auto loc = get_location_from_tag(tag);
    LOC_BAIL(loc);

    USE_THIS_SHADER();
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
  }

  void GL_Shader::set_integer(tag_t tag, int i)
  {
    auto loc = get_location_from_tag(tag);
    LOC_BAIL(loc);

    USE_THIS_SHADER();
    glUniform1i(loc, i);
  }

  void GL_Shader::set_vec2(tag_t tag, glm::vec2 const& v)
  {
    auto loc = get_location_from_tag(tag);
    LOC_BAIL(loc);

    USE_THIS_SHADER();
    glUniform2fv(loc, 1, &v[0]);
  }
  void GL_Shader::set_vec3(tag_t tag, glm::vec3 const& v)
  {
    auto loc = get_location_from_tag(tag);
    LOC_BAIL(loc);

    USE_THIS_SHADER();
    glUniform3fv(loc, 1, &v[0]);
  }
  void GL_Shader::set_vec4(tag_t tag, glm::vec4 const& v)
  {
    auto loc = get_location_from_tag(tag);
    LOC_BAIL(loc);

    USE_THIS_SHADER();
    glUniform4fv(loc, 1, &v[0]);
  }
  void GL_Shader::set_float(tag_t tag, float f)
  {
    auto loc = get_location_from_tag(tag);
    LOC_BAIL(loc);

    USE_THIS_SHADER();
    glUniform1f(loc, f);
  }
  void GL_Shader::use()
  {
    REDC_ASSERT_MSG(linked_, "Cannot activate a non-linked shader program");
    glUseProgram(prog_);
  }

  GLint GL_Shader::get_location_from_tag(tag_t tag)
  {
    if(tags.count(tag))
    {
      // If the tag exists return that value from the map
      return tags.at(tag);
    }
    // Otherwise we the tag isn't valid, return a bad location
    return 0;
  }
} } }

#undef LOC_BAIL
#undef ASSERT_FILE
#undef USE_THIS_SHADER
