/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "program.h"
#include <sstream>
#include <fstream>
#include "../../common/log.h"
namespace game
{
  namespace gfx
  {
    namespace gl
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
        program.vshade = glCreateShader(GL_VERTEX_SHADER);
        program.fshade = glCreateShader(GL_FRAGMENT_SHADER);

        compile_shader(program.vshade, v);
        compile_shader(program.fshade, f);

        program.program = glCreateProgram();
        glAttachShader(program.program, program.vshade);
        glAttachShader(program.program, program.fshade);
        glLinkProgram(program.program);

        GLint result = GL_FALSE;
        glGetProgramiv(program.program, GL_LINK_STATUS, &result);
        if(result == GL_FALSE)
        {
          // Compilation failed.
          constexpr size_t info_log_length = 2048;
          auto info_log = new char[info_log_length];

          glGetProgramInfoLog(program.program, info_log_length - 1, NULL,
                              info_log);

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
        glUseProgram(program);
      }
      GLint Program::get_uniform_location(std::string n) noexcept
      {
        return glGetUniformLocation(program, n.data());
      }
      Program::Program(Program&& p1) noexcept
                       : vshade(p1.vshade), fshade(p1.fshade),
                         program(p1.program)
      {
        p1.vshade = 0;
        p1.fshade = 0;
        p1.program = 0;
      }
      Program& Program::operator=(Program&& p1) noexcept
      {
        vshade = p1.vshade;
        fshade = p1.fshade;
        program = p1.program;

        p1.vshade = 0;
        p1.fshade = 0;
        p1.program = 0;

        return *this;
      }
      Program::~Program() noexcept
      {
  #ifndef SURVIVE_RELEASE_BUILD
        GLint current_program;
        glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
        if(current_program && static_cast<GLuint>(current_program) == program)
        {
          log_w("'%' (Shader program) destructed while in use; "
                "Removing binding", name);
          glUseProgram(0);
        }
  #endif

        if(program) glDeleteProgram(program);
        if(fshade) glDeleteShader(fshade);
        if(vshade) glDeleteShader(vshade);
      }
    }
  }
}
