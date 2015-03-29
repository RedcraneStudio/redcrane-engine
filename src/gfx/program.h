/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "glad/glad.h"

#include <glm/glm.hpp>

#include <istream>
namespace survive
{
  namespace gfx
  {
    struct Program
    {
      void use() noexcept;

      GLint get_uniform_location(std::string n) noexcept;

      bool set_uniform_mat4(GLint loc, glm::mat4 const& m) noexcept;
      bool set_uniform_int(GLint loc, int i) noexcept;

      static Program from_contents(std::string v, std::string f) noexcept;
      static Program from_streams(std::istream&& v, std::istream&& f) noexcept;
      static Program from_files(std::string v, std::string f) noexcept;

      Program(Program&&) noexcept;
      Program& operator=(Program&&) noexcept;
      ~Program() noexcept;

      std::string name;
    private:
      GLuint vshade_;
      GLuint fshade_;
      GLuint prog_;

      Program() = default;
    };
  }
}
