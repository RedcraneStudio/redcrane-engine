/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include <glm/glm.hpp>
#include "../common/color.h"
namespace game
{
  namespace gfx
  {
    struct Shader
    {
      // These functions should, if called more than once, discard the one it
      // was last initialized with.
      virtual void load_vertex_part(std::string const&) noexcept {}
      virtual void load_fragment_part(std::string const&) noexcept {}
      virtual void load_geometry_part(std::string const&) noexcept {}

      virtual int get_location(std::string const&) noexcept { return 0; }

      virtual void set_matrix(int, glm::mat4 const&) noexcept {}

      virtual void set_sampler(int, unsigned int) noexcept {}

      virtual void set_vec2(int, glm::vec2 const&) noexcept {}
      virtual void set_vec3(int, glm::vec3 const&) noexcept {}
      virtual void set_vec4(int, glm::vec4 const&) noexcept {}

      // Defaults to wrapping over set_vec4 converting the colors to
      // a floating point value out of 0xff.
      virtual void set_color(int, Color const&) noexcept;

      // Although consider the interface below mildly deprecated. In other
      // prefer some other form of uniform location caching.
      void set_diffuse_name(std::string const&) noexcept;
      void set_projection_name(std::string const&) noexcept;
      void set_view_name(std::string const&) noexcept;
      void set_model_name(std::string const&) noexcept;
      void set_sampler_name(std::string const&) noexcept;

      void set_diffuse(Color) noexcept;
      void set_projection(glm::mat4 const&) noexcept;
      void set_view(glm::mat4 const&) noexcept;
      void set_model(glm::mat4 const&) noexcept;
      void set_sampler(unsigned int) noexcept;

    private:
      int diffuse_loc_ = -1;
      int proj_loc_ = -1;
      int view_loc_ = -1;
      int model_loc_ = -1;
      int sampler_loc_ = -1;

      void set_loc_(int&, std::string const&) noexcept;
    };
  }
}
