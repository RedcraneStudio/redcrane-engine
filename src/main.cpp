/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include "glfw3.h"
#include "glm/glm.hpp"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

struct Mesh
{
  std::vector<glm::vec3> vertices;
  std::vector<std::tuple<int, int, int> > faces;
};

Mesh load_mesh(std::istream&& file) noexcept
{
  auto mesh = Mesh{};

  auto line = std::string{};
  while(!std::getline(file, line).eof())
  {
    if(line.size() == 0) continue;
    if(line[0] == '#') continue;

    std::istringstream stream{line};

    auto c = char{};
    stream >> c;

    if(c == 'v')
    {
      glm::vec3 v;
      stream >> v.x >> v.y >> v.z;
      mesh.vertices.push_back(v);
    }
    else if(c == 'f')
    {
      int f, s, t;
      stream >> f >> s >> t;
      mesh.faces.emplace_back(f, s, t);
    }
    else continue;
  }

  return mesh;
}

Mesh load_mesh_from_file(std::string const& file) noexcept
{
  std::ifstream stream{file};
  return load_mesh(std::move(stream));
}
Mesh load_mesh_from_data(std::string const& data) noexcept
{
  std::istringstream stream{data};;
  return load_mesh(std::move(stream));
}

TEST_CASE(".obj mesh is loaded", "[load_mesh]")
{
  std::string data =
  "v -10.0 10.0 0.0\n"
  "v -10.0 -10.0 0.0\n"
  "v 10.0 10.0 0.0\n"
  "v 10.0 -10.0 0.0\n"
  "f 2 4 3\n"
  "f 1 2 3\n";

  auto mesh = load_mesh_from_data(data);
  REQUIRE(mesh.vertices.size() == 4);
  REQUIRE(mesh.faces.size() == 2);
}

struct Command_Options
{
  bool test = false;
};

Command_Options parse_command_line(int argc, char** argv)
{
  Command_Options opt;
  for(int i = 0; i < argc; ++i)
  {
    auto option = argv[i];
    if(strcmp(option, "--test") == 0)
    {
      opt.test = true;
    }
  }
  return opt;
}

int main(int argc, char** argv)
{
}
