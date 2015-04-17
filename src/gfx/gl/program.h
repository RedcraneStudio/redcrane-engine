/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "glad/glad.h"

#include <glm/glm.hpp>

#include <istream>
namespace game
{
  namespace gfx
  {
    namespace gl
    {
      // Kind of a leaky abstraction since we need to set uniforms with
      // standard opengl functions, it's useful though in simply managing
      // resources and such.
      struct Program
      {
        void use() noexcept;

        GLint get_uniform_location(std::string n) noexcept;

        static Program from_contents(std::string v, std::string f) noexcept;
        static Program from_streams(std::istream&& v, std::istream&& f) noexcept;
        static Program from_files(std::string v, std::string f) noexcept;

        Program(Program&&) noexcept;
        Program(Program const&) noexcept = delete;
        Program& operator=(Program&&) noexcept;
        Program& operator=(Program const&) noexcept = delete;
        ~Program() noexcept;

        std::string name;

        GLuint vshade;
        GLuint fshade;
        GLuint program;

      private:
        Program() = default;
      };
    }
  }
}
