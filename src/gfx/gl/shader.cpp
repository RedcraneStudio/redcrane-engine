/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "shader.h"

#include <istream>
#include <fstream>
#include <string>
#include "glad/glad.h"

#include "../../common/log.h"
#include "driver.h"

#define LOC_BAIL(location) if(location == -1) return

#define USE_THIS_SHADER() driver_->use_shader(*this)

// TODO: Log bad uniform locations.

namespace game { namespace gfx { namespace gl
{
  std::string load_stream(std::istream& stream) noexcept
  {
    std::string ret;
    while(!stream.eof() && stream.good())
    {
      ret.push_back(stream.get());
    }
    return ret;
  }
  void compile_shader(GLuint shade, std::string const& data)
  {
    int len = data.size();
    auto data_cstr = data.data();
    glShaderSource(shade, 1, &data_cstr, &len);

    glCompileShader(shade);

    GLint result = 0;
    glGetShaderiv(shade, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE)
    {
      // Compilation failed.
      constexpr size_t info_log_length = 2048;
      auto info_log = new char[info_log_length];

      glGetShaderInfoLog(shade, info_log_length - 1, NULL, info_log);

      info_log[info_log_length - 1] = '\0';
      log_e("Shader compilation failed (info log):\n%", info_log);

      delete[] info_log;
    }
  }
  GL_Shader::GL_Shader(Driver& d) noexcept : driver_(&d)
  {
    prog_ = glCreateProgram();
  }
  GL_Shader::~GL_Shader() noexcept
  {
    if(prog_) glDeleteProgram(prog_);
    if(f_shade_) glDeleteShader(f_shade_);
    if(v_shade_) glDeleteShader(v_shade_);
  }
  void GL_Shader::load_vertex_part(std::string const& str) noexcept
  {
    v_shade_ = glCreateShader(GL_VERTEX_SHADER);
    glAttachShader(prog_, v_shade_);

    std::ifstream file{str};
    compile_shader(v_shade_, load_stream(file));

    try_load_();
  }
  void GL_Shader::load_fragment_part(std::string const& str) noexcept
  {
    f_shade_ = glCreateShader(GL_FRAGMENT_SHADER);
    glAttachShader(prog_, f_shade_);

    std::ifstream file{str};
    compile_shader(f_shade_, load_stream(file));

    try_load_();
  }

  void GL_Shader::try_load_() noexcept
  {
    if(!v_shade_ || !f_shade_) return;

    glLinkProgram(prog_);
    linked_ = true;
  }

  int GL_Shader::get_location(std::string const& str) noexcept
  {
    if(!linked_) return -1;
    return glGetUniformLocation(prog_, str.data());
  }

  void GL_Shader::set_matrix(int loc, glm::mat4 const& mat) noexcept
  {
    LOC_BAIL(loc);

    USE_THIS_SHADER();
    glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
  }

  void GL_Shader::set_integer(int loc, int tex_unit) noexcept
  {
    LOC_BAIL(loc);

    USE_THIS_SHADER();
    glUniform1i(loc, tex_unit);
  }

  void GL_Shader::set_vec2(int loc, glm::vec2 const& v) noexcept
  {
    LOC_BAIL(loc);

    USE_THIS_SHADER();
    glUniform2fv(loc, 1, &v[0]);
  }
  void GL_Shader::set_vec3(int loc, glm::vec3 const& v) noexcept
  {
    LOC_BAIL(loc);

    USE_THIS_SHADER();
    glUniform3fv(loc, 1, &v[0]);
  }
  void GL_Shader::set_vec4(int loc, glm::vec4 const& v) noexcept
  {
    LOC_BAIL(loc);

    USE_THIS_SHADER();
    glUniform4fv(loc, 1, &v[0]);
  }
  void GL_Shader::set_float(int loc, float f) noexcept
  {
    LOC_BAIL(loc);

    USE_THIS_SHADER();
    glUniform1f(loc, f);
  }
  void GL_Shader::use() noexcept
  {
    if(!linked_) return;
    glUseProgram(prog_);
  }
} } }

#undef LOC_BAIL
