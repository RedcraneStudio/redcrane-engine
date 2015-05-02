/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "mesh_load.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include "log.h"
namespace game
{
  bool operator<(Face const& f1, Face const& f2) noexcept
  {
    if(f1.vertex == f2.vertex)
    {
      if(f1.normal == f2.normal)
      {
        return f1.tex_coord < f2.tex_coord;
      }
      else return f1.normal < f2.normal;
    }
    else return f1.vertex < f2.vertex;
  }
  bool operator==(Face const& f1, Face const& f2) noexcept
  {
    return f1.vertex == f2.vertex && f1.normal == f2.normal &&
           f1.tex_coord == f2.tex_coord;
  }
  Face parse_face(std::string str) noexcept
  {
    // str could be "2" or "2/1" or "2//4" or "2/1/4"
    Face f{0, 0, 0};

    std::istringstream stream{str};

    std::string vert_str;
    std::getline(stream, vert_str, '/');

    std::string tex_str;
    std::getline(stream, tex_str, '/');

    std::string norm_str;
    std::getline(stream, norm_str, '/');

    try
    {
      if(!vert_str.empty()) f.vertex = std::stoi(vert_str);
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

    // Vector to be filled with all the unique face vertex/texture/normal
    // tuples.
    std::vector<Face> face_tuples;
    std::vector<Face> face_tuples_copy;

    // First pass, load everything.
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
      // Load the faces as a tuple into the faces_tuple vector.
      if(op == "f")
      {
        for(int i = 0; i < 3; ++i)
        {
          // Parse the single face.
          std::string face_str;
          line_stream >> face_str;
          // Push it to the vector
          face_tuples.push_back(parse_face(face_str));
        }
      }
    }

    // First copy the faces to preserve the original order, which is important
    // because pairs of threes represent triangles, obviously.
    face_tuples_copy = face_tuples;

    // Sort it
    using std::begin; using std::end;
    std::sort(begin(face_tuples), end(face_tuples));

    // Remove doubles
    auto new_end = std::unique(begin(face_tuples), end(face_tuples));
    face_tuples.erase(new_end, end(face_tuples));

    // Now duplicate vertices as needed.
    auto mesh = Mesh_Data{};

    // Go through the unique list and build each vertex.
    for(auto const& f : face_tuples)
    {
      Vertex vertex;
      if(f.vertex - 1 < vertices.size())
      {
        vertex.position = vertices[f.vertex - 1];
      }
      if(f.normal - 1 < normals.size())
      {
        vertex.normal = normals[f.normal - 1];
      }
      if(f.tex_coord - 1 < uv.size())
      {
        vertex.uv = uv[f.tex_coord - 1];
      }
      mesh.vertices.push_back(vertex);
    }

    // Now go through and get a suitable list of faces by matching each one
    // with the list of unique face tuples.
    for(auto const& real_face : face_tuples_copy)
    {
      for(size_t i = 0; i < face_tuples.size(); ++i)
      {
        if(face_tuples[i] == real_face)
        {
          mesh.elements.push_back(i);
        }
      }
    }
    m.prepare(std::move(mesh));
  }
}
