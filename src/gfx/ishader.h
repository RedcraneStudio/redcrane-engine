/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include <vector>
#include <istream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../common/color.h"
#include "../assets/live_file.h"

#include "common.h"
#include "ihandle.h"
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
    struct IShader : public IHandle
    {
      virtual ~IShader() {}

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

      // Using built-in tags
      virtual void set_vec2(tag_t, float const*);
      virtual void set_vec2(tag_t, glm::vec2 const& vec);

      virtual void set_vec3(tag_t, float const*);
      virtual void set_vec3(tag_t, glm::vec3 const& vec);

      virtual void set_vec4(tag_t, float const*);
      virtual void set_vec4(tag_t, glm::vec4 const& vec);

      virtual void set_ivec2(tag_t, int const*);
      virtual void set_ivec2(tag_t, glm::ivec2 const& vec);

      virtual void set_ivec3(tag_t, int const*);
      virtual void set_ivec3(tag_t, glm::ivec3 const& vec);

      virtual void set_ivec4(tag_t, int const*);
      virtual void set_ivec4(tag_t, glm::ivec4 const& vec);

      virtual void set_bvec2(tag_t, bool const*);
      virtual void set_bvec2(tag_t, glm::bvec2 const& vec);

      virtual void set_bvec3(tag_t, bool const*);
      virtual void set_bvec3(tag_t, glm::bvec3 const& vec);

      virtual void set_bvec4(tag_t, bool const*);
      virtual void set_bvec4(tag_t, glm::bvec4 const& vec);

      virtual void set_mat2(tag_t, float const*);
      virtual void set_mat2(tag_t, glm::mat2 const& mat);

      virtual void set_mat3(tag_t, float const*);
      virtual void set_mat3(tag_t, glm::mat3 const& mat);

      virtual void set_mat4(tag_t, float const*);
      virtual void set_mat4(tag_t, glm::mat4 const& mat);

      virtual void set_float(tag_t, float);
      virtual void set_integer(tag_t, int);
      virtual void set_bool(tag_t, bool);

      // Using param binds managed externally.
      virtual void set_vec2(Param_Bind, float const*) {}
      virtual void set_vec2(Param_Bind bind, glm::vec2 const& vec)
      { set_vec2(bind, glm::value_ptr(vec)); }

      virtual void set_vec3(Param_Bind, float const*) {}
      virtual void set_vec3(Param_Bind bind, glm::vec3 const& vec)
      { set_vec3(bind, glm::value_ptr(vec)); }

      virtual void set_vec4(Param_Bind, float const*) {}
      virtual void set_vec4(Param_Bind bind, glm::vec4 const& vec)
      { set_vec4(bind, glm::value_ptr(vec)); }

      virtual void set_ivec2(Param_Bind, int const*) {}
      virtual void set_ivec2(Param_Bind bind, glm::ivec2 const& vec)
      { set_ivec2(bind, glm::value_ptr(vec)); }

      virtual void set_ivec3(Param_Bind, int const*) {}
      virtual void set_ivec3(Param_Bind bind, glm::ivec3 const& vec)
      { set_ivec3(bind, glm::value_ptr(vec)); }

      virtual void set_ivec4(Param_Bind, int const*) {}
      virtual void set_ivec4(Param_Bind bind, glm::ivec4 const& vec)
      { set_ivec4(bind, glm::value_ptr(vec)); }

      virtual void set_bvec2(Param_Bind, bool const*) {}
      virtual void set_bvec2(Param_Bind bind, glm::bvec2 const& vec)
      { set_bvec2(bind, glm::value_ptr(vec)); }

      virtual void set_bvec3(Param_Bind, bool const*) {}
      virtual void set_bvec3(Param_Bind bind, glm::bvec3 const& vec)
      { set_bvec3(bind, glm::value_ptr(vec)); }

      virtual void set_bvec4(Param_Bind, bool const*) {}
      virtual void set_bvec4(Param_Bind bind, glm::bvec4 const& vec)
      { set_bvec4(bind, glm::value_ptr(vec)); }

      virtual void set_mat2(Param_Bind, float const*) {}
      virtual void set_mat2(Param_Bind bind, glm::mat2 const& mat)
      { set_mat2(bind, glm::value_ptr(mat)); }

      virtual void set_mat3(Param_Bind, float const*) {}
      virtual void set_mat3(Param_Bind bind, glm::mat3 const& mat)
      { set_mat3(bind, glm::value_ptr(mat)); }

      virtual void set_mat4(Param_Bind, float const*) {}
      virtual void set_mat4(Param_Bind bind, glm::mat4 const& mat)
      { set_mat4(bind, glm::value_ptr(mat)); }

      virtual void set_float(Param_Bind, float) {}
      virtual void set_integer(Param_Bind, int) {}
      virtual void set_bool(Param_Bind, bool) {}

      // Defaults to wrapping over set_vec4 converting the colors to
      // a floating point value out of 0xff.
      virtual void set_color(tag_t, Color const&);

      virtual Attrib_Bind get_attrib_bind(std::string attrib) const = 0;
      virtual Param_Bind get_param_bind(std::string param) const = 0;
      virtual Param_Bind get_tag_param_bind(std::string tag) const = 0;
    };

    struct Live_Shader
    {
      Live_Shader(std::unique_ptr<IShader> shade) : shader_(std::move(shade)) {}

      void set_vertex_file(std::string const& filename);
      void set_fragment_file(std::string const& filename);
      void set_geometry_file(std::string const& filename);

      void update_sources();
    private:
      std::unique_ptr<IShader> shader_;

      std::unique_ptr<Live_File> vertex_file_;
      std::unique_ptr<Live_File> fragment_file_;
      std::unique_ptr<Live_File> geometry_file_;
    };

    // Load shader source from file
    IShader::shader_source_t load_stream(std::istream& stream);
    IShader::shader_source_t load_file(std::string filename);

    // Load contents of a file into the respective shader part of a given shader
    // program.
    void load_vertex_file(IShader& shade, std::string filename);
    void load_fragment_file(IShader& shade, std::string filename);
    void load_geometry_file(IShader& shade, std::string filename);
  }
}
