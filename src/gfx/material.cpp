/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "material.h"
#include "../common/log.h"
namespace survive
{
  namespace gfx
  {
    void Material::set_program(std::shared_ptr<Program> prog) noexcept
    {
      program_ = prog;
    }
    bool Material::set_diffuse_uniform_name(std::string n) noexcept
    {
      return set_location_from_name_(n, diffuse_uniform_loc_, "diffuse color");
    }
    bool Material::set_sampler_uniform_name(std::string n) noexcept
    {
      return set_location_from_name_(n, sampler_uniform_loc_, "sampler");
    }
    void Material::set_diffuse_color(Color color) noexcept
    {
      diffuse_color_ = color;
      glUniform3f(diffuse_uniform_loc_, diffuse_color_.r / (float) 0xff,
                  diffuse_color_.g / (float) 0xff,
                  diffuse_color_.b / (float) 0xff);
    }
    bool Material::set_location_from_name_(std::string n, GLint& loc,
                                           std::string thing) noexcept
    {
      if(!program_)
      {
        log_w("Not setting % name to '%' since material doesn't "
              "have a program.", thing, n);
        return false;
      }

      loc = program_->get_uniform_location(n);

      return true;
    }
  }
}
