/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "mesh.h"
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include "common/log.h"

namespace strat
{
  namespace
  {
    struct Face
    {
      unsigned int vertex;
      unsigned int normal;
      unsigned int tex_coord;
    };

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
  }

  Mesh Mesh::from_contents(std::string str) noexcept
  {
    // Load the stream into a string and pass that to the function.
    std::istringstream stream{str};
    return Mesh::from_stream(std::move(stream));
  }

  Mesh Mesh::from_stream(std::istream&& stream) noexcept
  {
    // This mesh will store the intermediate vertices, normals, and texture
    // coordinates.
    auto basic_mesh = Mesh{};

    // Vector to be filled with all the unique face vertex/texture/normal
    // tuples.
    auto face_tuples = std::vector<Face>{};
    auto face_tuples_copy = std::vector<Face>{};

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
        basic_mesh.vertices.push_back(parse_vec3(line_stream));
      }
      if(op == "vn")
      {
        basic_mesh.normals.push_back(parse_vec3(line_stream));
      }
      if(op == "vt")
      {
        glm::vec2 coord;
        line_stream >> coord.x >> coord.y;
        basic_mesh.tex_coords.push_back(coord);
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
    auto mesh = Mesh{};

    // Go through the unique list and build each vertex.
    auto& verts = basic_mesh.vertices;
    auto& norms = basic_mesh.normals;
    auto& uvs = basic_mesh.tex_coords;
    for(auto const& f : face_tuples)
    {
      if(f.vertex - 1 < verts.size())
      {
        mesh.vertices.push_back(verts[f.vertex - 1]);
      }
      if(f.normal - 1 < norms.size())
      {
        mesh.normals.push_back(norms[f.normal - 1]);
      }
      if(f.tex_coord - 1 < uvs.size())
      {
        mesh.tex_coords.push_back(uvs[f.tex_coord - 1]);
      }
    }

    // Now go through and get a suitable list of faces by matching each one
    // with the list of unique face tuples.
    for(auto const& real_face : face_tuples_copy)
    {
      for(size_t i = 0; i < face_tuples.size(); ++i)
      {
        if(face_tuples[i] == real_face)
        {
          mesh.faces.push_back(i);
        }
      }
    }
    return mesh;
  }
  Mesh Mesh::from_file(std::string file) noexcept
  {
    std::ifstream stream{file};
    if(!stream.good())
    {
      log_w("File stream from '%' not good", file);
      return Mesh{};
    }

    return Mesh::from_stream(std::move(stream));
  }

  AABB generate_aabb(Mesh const& mesh) noexcept
  {
    glm::vec3 min;
    glm::vec3 max;

    for(auto const& coord : mesh.vertices)
    {
      min.x = std::min(min.x, coord.x);
      min.y = std::min(min.y, coord.y);
      min.z = std::min(min.z, coord.z);

      max.x = std::max(max.x, coord.x);
      max.y = std::max(max.y, coord.y);
      max.z = std::max(max.z, coord.z);
    }

    auto aabb = AABB{};
    aabb.width = std::abs(max.x - min.x);
    aabb.height = std::abs(max.y - min.y);
    aabb.depth = std::abs(max.z - min.z);

    aabb.min = min;

    return aabb;
  }
}


#include "catch/catch.hpp"

TEST_CASE("Face index string is properly parsed", "[struct Face]")
{
  using namespace strat;

  auto f = parse_face("6");
  REQUIRE(f.vertex == 6);
  REQUIRE(f.tex_coord == 0);
  REQUIRE(f.normal == 0);

  f = parse_face("5//2");
  REQUIRE(f.vertex == 5);
  REQUIRE(f.tex_coord == 0);
  REQUIRE(f.normal == 2);

  f = parse_face("7/1/5");
  REQUIRE(f.vertex == 7);
  REQUIRE(f.tex_coord == 1);
  REQUIRE(f.normal == 5);

  f = parse_face("9/8");
  REQUIRE(f.vertex == 9);
  REQUIRE(f.tex_coord == 8);
  REQUIRE(f.normal == 0);
}

TEST_CASE(".obj mesh is properly parsed", "[struct Mesh]")
{
  using namespace strat;

  std::string data =
  "v -10.0 10.0 0.0\n"
  "v -10.0 -10.0 0.0\n"
  "v 10.0 10.0 0.0\n"
  "v 10.0 -10.0 0.0\n"
  "f 2 4 3\n"
  "f 1 2 3\n";

  auto mesh = Mesh::from_stream(std::istringstream{data});
  REQUIRE(mesh.vertices.size() == 4);
  REQUIRE(mesh.faces.size() == 6);
}
