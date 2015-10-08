/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */

#include "catch/catch.hpp"

#include "common/tree.h"

TEST_CASE("tree returns valid iterators for allocated depth levels", "[Tree]")
{
  using namespace game;
  Quadtree<int> tree;

  tree.set_depth(3);

  // Check our sanity.
  REQUIRE(tree.level_begin(0) != tree.end());
  REQUIRE(tree.level_begin(1) != tree.end());
  REQUIRE(tree.level_begin(2) != tree.end());
  REQUIRE(tree.level_begin(3) == tree.end());

  SECTION("adding depth doesn't change previous levels")
  {
    constexpr int expect = 5;

    tree.level_begin(2)->val = expect;

    // Resize (specifically expand) the tree.
    tree.set_depth(4);

    // Make sure we actually did the allocation.
    REQUIRE(tree.level_begin(3) != tree.end());

    REQUIRE(tree.level_begin(2)->val == expect);

    SECTION("subtracting depth doesn't corrupt unchanged levels")
    {
      tree.set_depth(3);

      REQUIRE(tree.level_begin(2)->val == expect);
    }
  }
}
