/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "ishader.h"
#include <istream>
#include <fstream>
#include "../common/log.h"
namespace redc { namespace gfx
{
  void IShader::set_vec2(tag_t tag, float const* floats)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_vec2(bind, floats);
  }
  void IShader::set_vec2(tag_t tag, glm::vec2 const& vec)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_vec2(bind, vec);
  }

  void IShader::set_vec3(tag_t tag, float const* floats)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_vec3(bind, floats);
  }
  void IShader::set_vec3(tag_t tag, glm::vec3 const& vec)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_vec3(bind, vec);
  }

  void IShader::set_vec4(tag_t tag, float const* floats)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_vec4(bind, floats);
  }
  void IShader::set_vec4(tag_t tag, glm::vec4 const& vec)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_vec4(bind, vec);
  }

  void IShader::set_ivec2(tag_t tag, int const* vals)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_ivec2(bind, vals);
  }
  void IShader::set_ivec2(tag_t tag, glm::ivec2 const& vec)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_ivec2(bind, vec);
  }

  void IShader::set_ivec3(tag_t tag, int const* vals)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_ivec3(bind, vals);
  }
  void IShader::set_ivec3(tag_t tag, glm::ivec3 const& vec)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_ivec3(bind, vec);
  }

  void IShader::set_ivec4(tag_t tag, int const* vals)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_ivec4(bind, vals);
  }
  void IShader::set_ivec4(tag_t tag, glm::ivec4 const& vec)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_ivec4(bind, vec);
  }

  void IShader::set_bvec2(tag_t tag, bool const* vals)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_bvec2(bind, vals);
  }
  void IShader::set_bvec2(tag_t tag, glm::bvec2 const& vec)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_bvec2(bind, vec);
  }

  void IShader::set_bvec3(tag_t tag, bool const* vals)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_bvec3(bind, vals);
  }
  void IShader::set_bvec3(tag_t tag, glm::bvec3 const& vec)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_bvec3(bind, vec);
  }

  void IShader::set_bvec4(tag_t tag, bool const* vals)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_bvec4(bind, vals);
  }
  void IShader::set_bvec4(tag_t tag, glm::bvec4 const& vec)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_bvec4(bind, vec);
  }

  void IShader::set_mat2(tag_t tag, float const* floats)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_mat2(bind, floats);
  }
  void IShader::set_mat2(tag_t tag, glm::mat2 const& mat)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_mat2(bind, mat);
  }

  void IShader::set_mat3(tag_t tag, float const* floats)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_mat3(bind, floats);
  }
  void IShader::set_mat3(tag_t tag, glm::mat3 const& mat)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_mat3(bind, mat);
  }

  void IShader::set_mat4(tag_t tag, float const* floats)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_mat4(bind, floats);
  }
  void IShader::set_mat4(tag_t tag, glm::mat4 const& mat)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_mat4(bind, mat);
  }

  void IShader::set_float(tag_t tag, float val)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_float(bind, val);
  }
  void IShader::set_integer(tag_t tag, int val)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_integer(bind, val);
  }
  void IShader::set_bool(tag_t tag, bool val)
  {
    Param_Bind bind = get_tag_param_bind(tag);
    set_bool(bind, val);
  }

  void IShader::set_color(tag_t tag, Color const& c)
  {
    glm::vec4 v;
    v.r = c.r / (float) 0xff;
    v.g = c.g / (float) 0xff;
    v.b = c.b / (float) 0xff;
    v.a = c.a / (float) 0xff;
    set_vec4(tag, v);
  }
  std::string load_stream(std::istream& stream)
  {
    std::string ret;
    while(!stream.eof() && stream.good())
    {
      auto c = stream.get();
      if(std::istream::traits_type::not_eof(c))
      {
        ret.push_back(c);
      }
    }
    return ret;
  }

  IShader::shader_source_t load_file(std::string filename)
  {
    // Open the file
    auto file_st = std::ifstream(filename);
    if(!file_st.good())
    {
      log_e("Failed to open: '%'", filename);
    }
    // Load the contents
    auto source_str = load_stream(file_st);

    // Convert to a vector of characters
    using std::begin; using std::end;
    return std::vector<char>(begin(source_str), end(source_str));
  }

  void load_vertex_file(IShader& shade, std::string filename)
  {
    shade.load_vertex_part(load_file(filename), filename);
  }
  void load_fragment_file(IShader& shade, std::string filename)
  {
    shade.load_fragment_part(load_file(filename), filename);
  }
  void load_geometry_file(IShader& shade, std::string filename)
  {
    shade.load_geometry_part(load_file(filename), filename);
  }

} }
