/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "../common/algorithm.h"

#include "catch/catch.hpp"

TEST_CASE("Sorted insertion works", "Algorithm Lib")
{
  using namespace game;

  std::vector<int> vec;

  sorted_insert(vec, 5);
  sorted_insert(vec, 3);
  sorted_insert(vec, 1);
  sorted_insert(vec, 6);
  sorted_insert(vec, 4);

  REQUIRE(vec.size() == 5);

  REQUIRE(vec[0] == 1);
  REQUIRE(vec[1] == 3);
  REQUIRE(vec[2] == 4);
  REQUIRE(vec[3] == 5);
  REQUIRE(vec[4] == 6);
}
