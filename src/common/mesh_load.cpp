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
  bool operator<(Vertex_Indices const& v1, Vertex_Indices const& v2) noexcept
  {
    // This is to get correct sorting for the set of vertex indices, otherwise
    // we'll generate the id right before it becomes useless.
    return v1.index < v2.index;
  }
  bool operator==(Vertex_Indices const& v1, Vertex_Indices const& v2) noexcept
  {
    // Index is intentionally left out of this comparison.
    return v1.vertex == v2.vertex && v1.normal == v2.normal &&
           v1.tex_coord == v2.tex_coord;
  }
  Vertex_Indices parse_vertex_indices(std::string str) noexcept
  {
    // str could be "2" or "2/1" or "2//4" or "2/1/4"
    Vertex_Indices f{0, 0, 0, 0};

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

    // This will store all of our unique vertices.
    std::set<Vertex_Indices> indices_set;
    // This will store our vertices in order.
    std::vector<Vertex_Indices> indices;

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
      if(op == "f")
      {
        // For each tuple of indices in the face.
        for(int i = 0; i < 3; ++i)
        {
          std::string index_str;
          line_stream >> index_str;
          indices.push_back(parse_vertex_indices(index_str));
        }
      }
    }

    int cur_index = 0;
    // Copy our vertice indices from the ordered vector to a set.
    for(auto& vert : indices)
    {
      // Give it an id based on it's position.
      vert.index = cur_index;

      // Because id doesn't participate in equality testing the first one in
      // gets to keep it's id.
      // But because index *is* used for ordering, these will stay in order
      // so that index is actually relevant.
      auto res = indices_set.insert(vert);

      // If an insertion took place, increment our counter.
      if(std::get<1>(res)) ++cur_index;
    }

    // Now duplicate vertices as needed.
    auto mesh = Mesh_Data{};

    // Go through the unique list and build each vertex.
    for(auto const& f : indices_set)
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

    /*
     * Note: This is when face_tuples was a vector.
     * This is worst case O(n^2) assuming no duplicate vertices are used. Worst
     * case probably won't be happening all that often but it's still a very
     * large O(n*m) where m is the number of unique vertices and n is the total
     * number of faces. Something like that.
     */
#if 0
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
#endif

    for(auto const& vertex : indices)
    {
      // We need to find the index of our current element indices in our unique
      // list. That offset should be what we push to the elements vector.
      mesh.elements.push_back(indices_set.find(vertex)->index);
    }

    m.allocate_from(std::move(mesh));
  }
}
