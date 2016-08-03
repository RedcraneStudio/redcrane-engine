/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "../common/color.h"
#include "types.h"
namespace redc
{
  namespace gfx
  {
    namespace tags
    {
      constexpr const char* proj_tag = "projection";
      constexpr const char* view_tag = "view";
      constexpr const char* model_tag = "model";

      constexpr const char* diffuse_tag = "diffuse";

      constexpr const char* dif_tex_tag = "dif_tex";
      constexpr const char* bump_tex_tag = "bump_tex";

      constexpr const char* envmap_tag = "envmap";
    }
    struct Shader
    {
      virtual ~Shader() {}

      using shader_source_t = std::vector<char>;

      // These functions should, if called more than once, discard the last
      // shader source it was called it, ie this is not an append operation it
      // is a replace operation just like glShaderSource.
      virtual void load_vertex_part(shader_source_t const&,
                                    std::string const&) {}
      virtual void load_fragment_part(shader_source_t const&,
                                      std::string const&) {}
      virtual void load_geometry_part(shader_source_t const&,
                                      std::string const&) {}

      virtual bool link() { return false; }
      virtual bool linked() { return false; }

      using tag_t = std::string;
      virtual void set_var_tag(tag_t, std::string) {}

      // Creates a tag with the same name of a given variable.
      inline void tag_var(std::string tag)
      { set_var_tag(tag, tag); }

      virtual Param_Bind get_tag_bind(tag_t);

      virtual void set_mat4(tag_t, glm::mat4 const&) {}
      virtual void set_mat3(tag_t, glm::mat3 const&) {}

      virtual void set_integer(tag_t, int) {}

      virtual void set_vec2(tag_t, glm::vec2 const&) {}
      virtual void set_vec3(tag_t, glm::vec3 const&) {}
      virtual void set_vec4(tag_t, glm::vec4 const&) {}

      virtual void set_float(tag_t, float) {}

      // Defaults to wrapping over set_vec4 converting the colors to
      // a floating point value out of 0xff.
      virtual void set_color(tag_t, Color const&);
    };

    // Load shader source from file
    Shader::shader_source_t load_file(std::string filename);

    // Load contents of a file into the respective shader part of a given shader
    // program.
    void load_vertex_file(Shader& shade, std::string filename);
    void load_fragment_file(Shader& shade, std::string filename);
    void load_geometry_file(Shader& shade, std::string filename);
  }
}
