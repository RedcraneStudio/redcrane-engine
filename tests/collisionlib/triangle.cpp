/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "catch/catch.hpp"

#include "collisionlib/triangle_conversion.h"

TEST_CASE("make_triangle works", "[collisionlib]")
{
  using namespace redc;

  std::array<glm::vec3, 3> pts;

  pts[0] = glm::vec3(1.0f, 5.0f, 2.0f);
  pts[1] = glm::vec3(3.0f, 5.0f, 9.0f);
  pts[2] = glm::vec3(7.0f, 5.0f, 2.0f);

  glm::vec3 norm = glm::vec3(0.0f, 1.0f, 0.0f);

  collis::Triangle triangle = collis::make_triangle(pts, norm);

  REQUIRE(triangle.positions[0].x == Approx(pts[0].x));
  REQUIRE(triangle.positions[0].y == Approx(pts[0].y));
  REQUIRE(triangle.positions[0].z == Approx(pts[0].z));

  REQUIRE(triangle.positions[1].x == Approx(pts[1].x));
  REQUIRE(triangle.positions[1].y == Approx(pts[1].y));
  REQUIRE(triangle.positions[1].z == Approx(pts[1].z));

  REQUIRE(triangle.positions[2].x == Approx(pts[2].x));
  REQUIRE(triangle.positions[2].y == Approx(pts[2].y));
  REQUIRE(triangle.positions[2].z == Approx(pts[2].z));

  REQUIRE(triangle.normal.x == Approx(norm.x));
  REQUIRE(triangle.normal.y == Approx(norm.y));
  REQUIRE(triangle.normal.z == Approx(norm.z));

  SECTION("correct signed distance from triangle plane")
  {
    REQUIRE(plane_signed_distance(triangle, glm::vec3(0.0f, 5.0f, 0.0f)) ==
            Approx(0.0f));
    REQUIRE(plane_signed_distance(triangle, glm::vec3(0.0f, 6.0f, 0.0f)) ==
            Approx(1.0f));
    REQUIRE(plane_signed_distance(triangle, glm::vec3(0.0f, 3.0f, 0.0f)) ==
            Approx(-2.0f));
  }
}
