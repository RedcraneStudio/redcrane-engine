/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */

#include "catch/catch.hpp"

#include "common/tree.h"

TEST_CASE("tree returns valid iterators for allocated depth levels", "[tree]")
{
  using namespace game;
  Quadtree<int> tree;

  tree.set_depth(3);

  // Check our sanity.
  REQUIRE(tree.level_begin(0) != tree.end());
  REQUIRE(tree.level_begin(1) != tree.end());
  REQUIRE(tree.level_begin(2) != tree.end());
  REQUIRE(tree.level_begin(3) == tree.end());

  REQUIRE(tree.begin() == tree.level_begin(0));

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

  SECTION("iterators to begin and end of level work")
  {
    // The iterator to the beginning of the first level should definitely be
    // equal to the very beginning of the tree.
    REQUIRE(tree.level_begin(0) == tree.begin());

    // Level zero always contains a single node.
    REQUIRE(tree.level_end(0) == tree.begin()+1);

    // For a quadtree, the second level should start at 1 and end at 5
    REQUIRE(tree.level_begin(1) == tree.begin()+1);
    REQUIRE(tree.level_end(1) == tree.begin()+5);

    REQUIRE(tree.level_begin(2) == tree.begin()+5);
    REQUIRE(tree.level_end(2) == tree.begin()+21);
  }
  SECTION("indexing works as expected")
  {
    constexpr int expect = 3;

    tree.node_at_depth(2, 0).val = expect;
    REQUIRE(tree.level_begin(2)->val == expect);

    tree.node_at_depth(0, 0).val = expect;
    REQUIRE(tree.level_begin(0)->val == expect);

    tree.node_at_depth(1, 1).val = expect;
    REQUIRE((tree.begin()+2)->val == expect);

    // This is pretty weird usage, but might be found in the wild
    tree.node_at_depth(0, 1).val = expect;
    REQUIRE(tree.level_begin(1)->val == expect);
    REQUIRE((tree.level_begin(0) + 1)->val == expect);

    // Test that same usage in a different way.
    tree.node_at_depth(1, 4).val = expect;
    REQUIRE(tree.level_begin(2)->val == expect);
    REQUIRE((tree.level_begin(1) + 4)->val == expect);
    REQUIRE((tree.level_begin(0) + 5)->val == expect);
  }
}
TEST_CASE("depth level is properly maintained in nodes", "[tree]")
{
  using namespace game;
  struct null_t {};

  // The tree has 5 children per node per depth level.
  Tree<null_t, 5> tree;

  // Allocate a single depth level.
  tree.set_depth(1);
  REQUIRE(tree.begin()[0].depth() == 0);

  // Add two more levels
  tree.set_depth(3);

  // Test each one

  REQUIRE(tree.begin()[0].depth() == 0);
  for(auto iter = tree.level_begin(1); iter < tree.level_end(1); ++iter)
  {
    REQUIRE(iter->depth() == 1);
  }
  for(auto iter = tree.level_begin(2); iter < tree.level_end(2); ++iter)
  {
    REQUIRE(iter->depth() == 2);
  }

  // Subtract a level
  tree.set_depth(2);

  REQUIRE(tree.begin()[0].depth() == 0);
  for(auto iter = tree.level_begin(1); iter < tree.level_end(1); ++iter)
  {
    REQUIRE(iter->depth() == 1);
  }

  // Get rid of everything
  tree.set_depth(0);

  // Add a bunch of levels this time
  tree.set_depth(5);

  // Test them all
  REQUIRE(tree.begin()[0].depth() == 0);
  for(auto iter = tree.level_begin(1); iter < tree.level_end(1); ++iter)
  {
    REQUIRE(iter->depth() == 1);
  }
  for(auto iter = tree.level_begin(2); iter < tree.level_end(2); ++iter)
  {
    REQUIRE(iter->depth() == 2);
  }
  for(auto iter = tree.level_begin(3); iter < tree.level_end(3); ++iter)
  {
    REQUIRE(iter->depth() == 3);
  }
  for(auto iter = tree.level_begin(4); iter < tree.level_end(4); ++iter)
  {
    REQUIRE(iter->depth() == 4);
  }

  // That's probably fine :)
}
