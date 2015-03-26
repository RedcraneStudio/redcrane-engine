/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "mesh.h"
#include <fstream>
#include <sstream>

namespace survive
{
  Mesh Mesh::from_stream(std::istream&& stream) noexcept
  {
    auto mesh = Mesh{};

    auto line = std::string{};
    while(!std::getline(stream, line).eof())
    {
      if(line.size() == 0) continue;
      if(line[0] == '#') continue;

      std::istringstream line_stream{line};

      // Read the first command char or whatever they call it.
      auto c = char{};
      line_stream >> c;

      if(c == 'v')
      {
        glm::vec3 v;
        line_stream >> v.x >> v.y >> v.z;
        mesh.vertices.push_back(v);
      }
      else if(c == 'f')
      {
        int f, s, t;
        line_stream >> f >> s >> t;
        mesh.faces.emplace_back(f - 1, s - 1, t - 1);
      }
      else continue;
    }

    return mesh;
  }
  Mesh Mesh::from_file(std::string file) noexcept
  {
    return Mesh::from_stream(std::ifstream(file));
  }
}

#include "catch/catch.hpp"

TEST_CASE(".obj mesh is properly parsed", "[struct Mesh]")
{
  using namespace survive;

  std::string data =
  "v -10.0 10.0 0.0\n"
  "v -10.0 -10.0 0.0\n"
  "v 10.0 10.0 0.0\n"
  "v 10.0 -10.0 0.0\n"
  "f 2 4 3\n"
  "f 1 2 3\n";

  auto mesh = Mesh::from_stream(std::istringstream{data});
  REQUIRE(mesh.vertices.size() == 4);
  REQUIRE(mesh.faces.size() == 2);
}
