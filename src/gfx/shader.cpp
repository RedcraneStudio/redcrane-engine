/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "shader.h"
namespace redc { namespace gfx
{
  void Shader::set_color(int loc, Color const& c) noexcept
  {
    glm::vec4 v;
    v.r = c.r / (float) 0xff;
    v.g = c.g / (float) 0xff;
    v.b = c.b / (float) 0xff;
    v.a = c.a / (float) 0xff;
    set_vec4(loc, v);
  }
  void Shader::set_loc_(int& loc, std::string const& str) noexcept
  {
    loc = get_location(str);
  }
  void Shader::set_diffuse_name(std::string const& str) noexcept
  {
    set_loc_(diffuse_loc_, str);
  }
  void Shader::set_projection_name(std::string const& str) noexcept
  {
    set_loc_(proj_loc_, str);
  }
  void Shader::set_view_name(std::string const& str) noexcept
  {
    set_loc_(view_loc_, str);
  }
  void Shader::set_model_name(std::string const& str) noexcept
  {
    set_loc_(model_loc_, str);
  }
  void Shader::set_sampler_name(std::string const& str) noexcept
  {
    set_loc_(sampler_loc_, str);
  }

  void Shader::set_diffuse(Color c) noexcept
  {
    set_color(diffuse_loc_, c);
  }
  void Shader::set_projection(glm::mat4 const& mat) noexcept
  {
    set_matrix(proj_loc_, mat);
  }
  void Shader::set_view(glm::mat4 const& mat) noexcept
  {
    set_matrix(view_loc_, mat);
  }
  void Shader::set_model(glm::mat4 const& mat) noexcept
  {
    set_matrix(model_loc_, mat);
  }
  void Shader::set_sampler(int unit) noexcept
  {
    set_integer(sampler_loc_, unit);
  }
} }
