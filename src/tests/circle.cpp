/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "../common/circle.h"

#include "catch/catch.hpp"

TEST_CASE("Circle-point detection works", "[struct Circle]")
{
  using namespace redc;

  Circle<float> circle;
  circle.center = {0.0f, 0.0f};
  circle.radius = 50.0f;

  REQUIRE(is_in(circle, {0.0f, 0.0f}));
  REQUIRE(is_in(circle, {50.0f, 0.0f}));
  REQUIRE(is_in(circle, {00.0f, 50.0f}));
  REQUIRE(is_in(circle, {25.0f, 25.0f}));

  REQUIRE(is_in(circle, {-25.0f, -25.0f}));
  REQUIRE(is_in(circle, {-50.0f, 0.0f}));
  REQUIRE(is_in(circle, {00.0f, -50.0f}));

  REQUIRE(!is_in(circle, {50.0f, 50.0f}));
  REQUIRE(!is_in(circle, {-50.0f, -50.0f}));

  REQUIRE(!is_in(circle, {1000.0f, -0.0f}));
  REQUIRE(!is_in(circle, {0.0f, 1000.0f}));

  REQUIRE(!is_in(circle, {-1000.0f, -0.0f}));
  REQUIRE(!is_in(circle, {0.0f, -1000.0f}));
}

TEST_CASE("Arc-point detection works", "[struct Arc]")
{
  using namespace redc;

  Arc<float> arc;
  arc.center = {0.0f, 0.0f};
  arc.radius = 50.0f;

  arc.start_radians = M_PI / 2;
  arc.end_radians = M_PI * 3 / 2;

  REQUIRE(is_in(arc, {0.0f, 0.0f}));

  REQUIRE(is_in(arc, {-49.0f, 0.0f}));
  REQUIRE(is_in(arc, {-5.0f, 35.0f}));
  REQUIRE(is_in(arc, {-5.0f, -35.0f}));

  REQUIRE(is_in(arc, {-25.0f, 25.0f}));
  REQUIRE(is_in(arc, {-25.0f, -25.0f}));

  REQUIRE(!is_in(arc, {50.0f, 0.0f}));

  REQUIRE(!is_in(arc, {50.0f, 50.0f}));
  REQUIRE(!is_in(arc, {-50.0f, -50.0f}));

  REQUIRE(!is_in(arc, {1000.0f, -0.0f}));
  REQUIRE(!is_in(arc, {0.0f, 1000.0f}));

  REQUIRE(!is_in(arc, {-1000.0f, -0.0f}));
  REQUIRE(!is_in(arc, {0.0f, -1000.0f}));

  arc.end_radians = M_PI;

  REQUIRE(is_in(arc, {-5.0f, 35.0f}));
  REQUIRE(!is_in(arc, {-5.0f, -35.0f}));

  arc.start_radians = M_PI * 3 / 2; // 270 deg
  arc.end_radians = M_PI / 2; // 90 deg

  REQUIRE(is_in(arc, {35.0f, 0.0f}));
  REQUIRE(is_in(arc, {5.0f, 35.0f}));
  REQUIRE(is_in(arc, {5.0f, -35.0f}));

  REQUIRE(!is_in(arc, {-35.0f, 0.0f}));
  REQUIRE(!is_in(arc, {-5.0f, 35.0f}));
  REQUIRE(!is_in(arc, {-5.0f, -35.0f}));
}
