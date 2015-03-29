/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include "../color.h"
#include "program.h"
namespace survive
{
  namespace gfx
  {
    struct Material
    {
      void set_program(std::shared_ptr<Program> prog) noexcept;

      bool set_diffuse_uniform_name(std::string n) noexcept;
      bool set_sampler_uniform_name(std::string n) noexcept;

      void set_diffuse_color(Color color) noexcept;
    private:
      std::shared_ptr<Program> program_;

      bool set_location_from_name_(std::string n, GLint& loc,
                                   std::string thing) noexcept;

      GLint diffuse_uniform_loc_ = 0;
      GLint sampler_uniform_loc_ = 0;

      Color diffuse_color_;
    };
  }
}
