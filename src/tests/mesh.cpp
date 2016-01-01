/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */

#include "../gfx/mesh.h"
#include "../gfx/support/load_wavefront.h"

#include <sstream>

#include "catch/catch.hpp"

TEST_CASE("Face index string is properly parsed", "[struct Face]")
{
  using namespace redc;

  using gfx::parse_wavefront_vert_ref;

  auto f = parse_wavefront_vert_ref("6");
  REQUIRE(f.position.value() == 5);
  REQUIRE_FALSE(f.tex_coord);
  REQUIRE_FALSE(f.normal);

  f = parse_wavefront_vert_ref("5//2");
  REQUIRE(f.position.value() == 4);
  REQUIRE_FALSE(f.tex_coord);
  REQUIRE(f.normal.value() == 1);

  f = parse_wavefront_vert_ref("7/1/5");
  REQUIRE(f.position.value() == 6);
  REQUIRE(f.tex_coord.value() == 0);
  REQUIRE(f.normal.value() == 4);

  f = parse_wavefront_vert_ref("9/8");
  REQUIRE(f.position.value() == 8);
  REQUIRE(f.tex_coord.value() == 7);
  REQUIRE_FALSE(f.normal);
}

TEST_CASE(".obj mesh is properly parsed", "[struct Mesh]")
{
  using namespace redc;

  std::string data =
  "v -10.0 10.0 0.0\n"
  "v -10.0 -10.0 0.0\n"
  "v 10.0 10.0 0.0\n"
  "v 10.0 -10.0 0.0\n"
  "f 2 4 3\n"
  "f 1 2 3\n";

  std::istringstream stream{data};
  auto mesh_data = gfx::load_wavefront(stream);

  REQUIRE(mesh_data.positions.size() == 4);
  REQUIRE(mesh_data.indices.size() == 6);

  glm::vec3 pt;
  pt = {-10.0, 10.0, 0.0};
  REQUIRE(mesh_data.positions[0] == pt);

  pt = {-10.0, -10.0, 0.0};
  REQUIRE(mesh_data.positions[1] == pt);

  pt = {10.0, 10.0, 0.0};
  REQUIRE(mesh_data.positions[2] == pt);

  pt = {10.0, -10.0, 0.0};
  REQUIRE(mesh_data.positions[3] == pt);

  REQUIRE(mesh_data.indices[0] == mesh_data.indices[4]);
  REQUIRE(mesh_data.indices[2] == mesh_data.indices[5]);
}
