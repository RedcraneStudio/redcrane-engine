/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */

#include "../collisionlib/triangle_conversion.h"

#include "catch/catch.hpp"

TEST_CASE("Correct triangle normals are generated",
          "[triangles_from_mesh_data]")
{
  using namespace redc;
  Indexed_Mesh_Data md;

  Vertex v;

  // These must be given in ccw order.
  v.position = glm::vec3(0.0f, 0.0f, 0.0f);
  md.vertices.push_back(v);

  v.position = glm::vec3(0.5f, 0.0f, 1.0f);
  md.vertices.push_back(v);

  v.position = glm::vec3(1.0f, 0.0f, 0.0f);
  md.vertices.push_back(v);

  md.elements.push_back(0);
  md.elements.push_back(1);
  md.elements.push_back(2);

  auto triangles = std::vector<collis::Triangle>{};
  collis::append_triangles(triangles, md, true);

  REQUIRE(triangles.size() == 1);

  // We probably shouldn't be doing this
  REQUIRE(triangles[0].normal.x == 0.0f);
  REQUIRE(triangles[0].normal.y == 1.0f);
  REQUIRE(triangles[0].normal.z == 0.0f);
}
