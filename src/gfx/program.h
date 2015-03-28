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

      bool set_uniform_mat4(std::string str, glm::mat4 const& m) noexcept;
      bool set_uniform_int(std::string str, int i) noexcept;

      static Program from_contents(std::string v, std::string f) noexcept;
      static Program from_streams(std::istream&& v, std::istream&& f) noexcept;
      static Program from_files(std::string v, std::string f) noexcept;

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
