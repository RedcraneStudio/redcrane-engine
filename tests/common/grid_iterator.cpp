/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "common/grid_iterator.h"
#include "catch/catch.hpp"

using namespace redc;

TEST_CASE("Grid iterator works", "[Grid_Iterator]")
{
  auto gi = make_grid_iterator(2, 2);
  auto iter = gi.begin();
  REQUIRE(iter->col == 0);
  REQUIRE(iter->row == 0);
  ++iter;
  REQUIRE(iter->col == 1);
  REQUIRE(iter->row == 0);
  ++iter;
  REQUIRE(iter->col == 0);
  REQUIRE(iter->row == 1);
  ++iter;
  REQUIRE(iter->col == 1);
  REQUIRE(iter->row == 1);
  ++iter;

  REQUIRE(iter == gi.end());
}

// TODO: Test the operator-- function.

// TODO: Make these tests WORK!
