/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "program.h"
#include <sstream>
#include <fstream>
#include "../common/log.h"
namespace survive
{
  namespace gfx
  {
    std::string load_stream(std::istream&& stream) noexcept
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

    Program Program::from_contents(std::string v, std::string f) noexcept
    {
      auto program = Program{};
      program.vshade_ = glCreateShader(GL_VERTEX_SHADER);
      program.fshade_ = glCreateShader(GL_FRAGMENT_SHADER);

      compile_shader(program.vshade_, v);
      compile_shader(program.fshade_, f);

      program.prog_ = glCreateProgram();
      glAttachShader(program.prog_, program.vshade_);
      glAttachShader(program.prog_, program.fshade_);
      glLinkProgram(program.prog_);

      GLint result = GL_FALSE;
      glGetProgramiv(program.prog_, GL_LINK_STATUS, &result);
      if(result == GL_FALSE)
      {
        // Compilation failed.
        constexpr size_t info_log_length = 2048;
        auto info_log = new char[info_log_length];

        glGetProgramInfoLog(program.prog_, info_log_length - 1, NULL, info_log);

        info_log[info_log_length - 1] = '\0';
        log_e("Program linking failed (info log):\n%", info_log);

        delete[] info_log;
      }

      return program;
    }

    Program Program::from_streams(std::istream&& v, std::istream&& f) noexcept
    {
      return Program::from_contents(load_stream(std::move(v)),
                                    load_stream(std::move(f)));
    }
    Program Program::from_files(std::string vs, std::string fs) noexcept
    {
      std::ifstream vsfile{vs};
      std::ifstream fsfile{fs};
      return Program::from_streams(std::move(vsfile), std::move(fsfile));
    }
    void Program::use() noexcept
    {
      glUseProgram(prog_);
    }
    bool Program::set_uniform_mat4(std::string n, glm::mat4 const& m) noexcept
    {
      // Cache this
      auto loc = glGetUniformLocation(prog_, n.data());
      glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);

      // Always succeeds for now.
      return true;
    }
    bool Program::set_uniform_int(std::string str, int i) noexcept
    {
      auto loc = glGetUniformLocation(prog_, str.data());
      glUniform1i(loc, i);

      return true;
    }
    Program::~Program() noexcept
    {
#ifndef SURVIVE_RELEASE_BUILD
      GLint current_program;
      glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
      if(static_cast<GLuint>(current_program) == prog_)
      {
        log_w("% (Shader program) destructed while in use;"
              "Removing binding", name);
        glUseProgram(0);
      }
#endif

      if(prog_) glDeleteProgram(prog_);
      if(fshade_) glDeleteProgram(fshade_);
      if(vshade_) glDeleteProgram(vshade_);
    }
  }
}
