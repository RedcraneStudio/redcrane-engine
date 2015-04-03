/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
// NOT COMPILED DON'T USE
#pragma once
#include <memory>
#include "program.h"
namespace survive
{
  namespace gfx
  {
    namespace gl
    {
      // This isn't thread safe, but will allow for a cache of programs.
      std::shared_ptr<Program> load_program(std::string decl) noexcept;
    }
  }
}

