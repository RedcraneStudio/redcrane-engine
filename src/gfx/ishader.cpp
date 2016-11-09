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

  void Live_Shader::set_vertex_file(std::string const& filename)
  {
    vertex_file_ = std::make_unique<Live_File>(filename);
  }

  void Live_Shader::set_fragment_file(std::string const& filename)
  {
    fragment_file_ = std::make_unique<Live_File>(filename);
  }
  void Live_Shader::set_geometry_file(std::string const& filename)
  {
    geometry_file_ = std::make_unique<Live_File>(filename);
  }

  void Live_Shader::update_sources()
  {
    bool needs_link = false;
    if(vertex_file_->changed_on_disk())
    {
      std::ifstream stream = vertex_file_->open_ifstream();
      shader_->load_vertex_part(load_stream(stream),
                                vertex_file_->filename());
      needs_link = true;
    }
    if(fragment_file_->changed_on_disk())
    {
      std::ifstream stream = fragment_file_->open_ifstream();
      shader_->load_fragment_part(load_stream(stream),
                                  fragment_file_->filename());
      needs_link = true;
    }
    if(geometry_file_->changed_on_disk())
    {
      std::ifstream stream = geometry_file_->open_ifstream();
      shader_->load_geometry_part(load_stream(stream),
                                  geometry_file_->filename());
      needs_link = true;
    }
    if(needs_link)
    {
      shader_->link();
    }
  }

  // Consumes the stream
  IShader::shader_source_t load_stream(std::istream& stream)
  {
    std::string source_str;
    while(!stream.eof() && stream.good())
    {
      auto c = stream.get();
      if(std::istream::traits_type::not_eof(c))
      {
        source_str.push_back(c);
      }
    }
    // Convert to a vector of characters
    using std::begin; using std::end;
    return std::vector<char>(begin(source_str), end(source_str));
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
    return load_stream(file_st);
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
