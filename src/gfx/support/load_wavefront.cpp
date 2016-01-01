/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "load_wavefront.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../../common/log.h"
namespace redc { namespace gfx
{
  Vert_Ref parse_wavefront_vert_ref(std::string str) noexcept
  {
    // str could be "2" or "2/1" or "2//4" or "2/1/4"
    Vert_Ref f;

    std::istringstream stream{str};

    std::string vert_str;
    std::getline(stream, vert_str, '/');

    std::string tex_str;
    std::getline(stream, tex_str, '/');

    std::string norm_str;
    std::getline(stream, norm_str, '/');

    try
    {
      if(!vert_str.empty()) f.position = std::stoi(vert_str) - 1;
    } catch(...) {}
    try
    {
      if(!tex_str.empty()) f.tex_coord = std::stoi(tex_str) - 1;
    } catch(...) {}
    try
    {
      if(!norm_str.empty()) f.normal = std::stoi(norm_str) - 1;
    } catch(...) {}

    return f;
  }
  glm::vec3 parse_vec3(std::istream& stream) noexcept
  {
    glm::vec3 t;
    stream >> t.x >> t.y >> t.z;
    return t;
  }

  Indexed_Split_Mesh_Data load_wavefront(std::istream& stream) noexcept
  {
    Indexed_Split_Mesh_Data ret{};

    std::string line;
    while(!std::getline(stream, line).eof() && stream.good())
    {
      if(line.size() == 0) continue;

      std::istringstream line_stream{line};

      // Operation
      std::string op;
      line_stream >> op;

      // Load vertices, normals and texture coordinates into basic_mesh.
      if(op == "v")
      {
        ret.positions.push_back(parse_vec3(line_stream));
      }
      if(op == "vn")
      {
        ret.normals.push_back(parse_vec3(line_stream));
      }
      if(op == "vt")
      {
        glm::vec2 coord;
        line_stream >> coord.x >> coord.y;
        ret.tex_coords.push_back(coord);
      }
      if(op == "f")
      {
        // For each tuple of indices in the face.
        for(int i = 0; i < 3; ++i)
        {
          std::string index_str;
          line_stream >> index_str;
          ret.indices.push_back(parse_wavefront_vert_ref(index_str));
        }
      }
    }

    return ret;
  }
  Indexed_Split_Mesh_Data load_wavefront(std::string file) noexcept
  {
    // Load us a stream.
    std::ifstream stream{file};
    if(!stream.good())
    {
      // Fuck. Abort.
      log_w("File stream from '%' not good", file);
      return Indexed_Split_Mesh_Data{};
    }
    return load_wavefront(stream);
  }
} }
