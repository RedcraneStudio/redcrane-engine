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

  auto f = parse_vert_ref("6");
  REQUIRE(f.position.value() == 6);
  REQUIRE_FALSE(f.tex_coord);
  REQUIRE_FALSE(f.normal);

  f = parse_vert_ref("5//2");
  REQUIRE(f.position.value() == 5);
  REQUIRE_FALSE(f.tex_coord);
  REQUIRE(f.normal.value() == 2);

  f = parse_vert_ref("7/1/5");
  REQUIRE(f.position.value() == 7);
  REQUIRE(f.tex_coord.value() == 1);
  REQUIRE(f.normal.value() == 5);

  f = parse_vert_ref("9/8");
  REQUIRE(f.position.value() == 9);
  REQUIRE(f.tex_coord.value() == 8);
  REQUIRE_FALSE(f.normal);
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

  glm::vec3 pt;
  pt = {-10.0, -10.0, 0.0};
  REQUIRE(mesh.mesh_data().vertices[0].position == pt);
  pt = {10.0, -10.0, 0.0};
  REQUIRE(mesh.mesh_data().vertices[1].position == pt);
  pt = {10.0, 10.0, 0.0};
  REQUIRE(mesh.mesh_data().vertices[2].position == pt);
  pt = {-10.0, 10.0, 0.0};
  REQUIRE(mesh.mesh_data().vertices[3].position == pt);

  REQUIRE(mesh.mesh_data().elements[0] == mesh.mesh_data().elements[4]);
  REQUIRE(mesh.mesh_data().elements[2] == mesh.mesh_data().elements[5]);
}
