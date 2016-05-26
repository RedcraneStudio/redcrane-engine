/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "shader.h"
#include <istream>
#include <fstream>
namespace redc { namespace gfx
{
  void Shader::set_color(tag_t tag, Color const& c)
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

  Shader::shader_source_t load_file(std::string filename)
  {
    // Open the file
    auto file_st = std::ifstream(filename);
    // Load the contents
    auto source_str = load_stream(file_st);

    // Convert to a vector of characters
    using std::begin; using std::end;
    return std::vector<char>(begin(source_str), end(source_str));
  }

  void load_vertex_file(Shader& shade, std::string filename)
  {
    shade.load_vertex_part(load_file(filename), filename);
  }
  void load_fragment_file(Shader& shade, std::string filename)
  {
    shade.load_fragment_part(load_file(filename), filename);
  }
  void load_geometry_file(Shader& shade, std::string filename)
  {
    shade.load_geometry_part(load_file(filename), filename);
  }

} }
