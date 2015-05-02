/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */

#include "../common/mesh.h"
#include "../common/mesh_load.h"
#include "../common/software_mesh.h"

#include <sstream>

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

TEST_CASE("Face index string is properly parsed", "[struct Face]")
{
  using namespace game;

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
  using namespace game;

  std::string data =
  "v -10.0 10.0 0.0\n"
  "v -10.0 -10.0 0.0\n"
  "v 10.0 10.0 0.0\n"
  "v 10.0 -10.0 0.0\n"
  "f 2 4 3\n"
  "f 1 2 3\n";

  Software_Mesh mesh;
  std::istringstream stream{data};
  load_obj(stream, mesh);
  REQUIRE(mesh.mesh_data().vertices.size() == 4);
  REQUIRE(mesh.mesh_data().elements.size() == 6);
}
