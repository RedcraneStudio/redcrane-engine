/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "mesh_load.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>
#include "log.h"
namespace game
{
  Vert_Ref parse_vert_ref(std::string str) noexcept
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
      if(!vert_str.empty()) f.position = std::stoi(vert_str);
    } catch(...) {}
    try
    {
      if(!tex_str.empty()) f.tex_coord = std::stoi(tex_str);
    } catch(...) {}
    try
    {
      if(!norm_str.empty()) f.normal = std::stoi(norm_str);
    } catch(...) {}

    return f;
  }
  glm::vec3 parse_vec3(std::istream& stream) noexcept
  {
    glm::vec3 t;
    stream >> t.x >> t.y >> t.z;
    return t;
  }

  void load_obj(std::string file, Mesh& m) noexcept
  {
    // Load us a stream.
    std::ifstream stream{file};
    if(!stream.good())
    {
      // Fuck. Abort.
      log_w("File stream from '%' not good", file);
      return;
    }
    load_obj(stream, m);
  }
  void load_obj(std::istream& stream, Mesh& m) noexcept
  {
    // This mesh will store the intermediate vertices, normals, and texture
    // coordinates.
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uv;
    std::vector<Vert_Ref> indices;

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
        vertices.push_back(parse_vec3(line_stream));
      }
      if(op == "vn")
      {
        normals.push_back(parse_vec3(line_stream));
      }
      if(op == "vt")
      {
        glm::vec2 coord;
        line_stream >> coord.x >> coord.y;
        uv.push_back(coord);
      }
      if(op == "f")
      {
        // For each tuple of indices in the face.
        for(int i = 0; i < 3; ++i)
        {
          std::string index_str;
          line_stream >> index_str;
          indices.push_back(parse_vert_ref(index_str));
        }
      }
    }

    auto mesh = make_optimized_mesh_data(indices, vertices, normals, uv);
    m.allocate_from(std::move(mesh));
  }
}
