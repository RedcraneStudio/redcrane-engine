/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "glad/glad.h"

#include <glm/glm.hpp>

#include <istream>
namespace survive
{
  namespace gfx
  {
    namespace gl
    {
      struct Program
      {
        void use() noexcept;

        GLint get_uniform_location(std::string n) noexcept;

        static Program from_contents(std::string v, std::string f) noexcept;
        static Program from_streams(std::istream&& v, std::istream&& f) noexcept;
        static Program from_files(std::string v, std::string f) noexcept;

        Program(Program&&) noexcept;
        Program& operator=(Program&&) noexcept;
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
