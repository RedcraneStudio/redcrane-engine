/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */

#include <string>
#include "common/volume.h"

#include "catch/catch.hpp"

#include "terrain/chunks.h"

TEST_CASE("gen_levels_volumes works as expected", "[terrainlib]")
{
  using namespace game;

  Quadtree<terrain::Chunk> tree;
  terrain::set_levels_volumes(tree, Vec<int>{2048,2048}, 3);

  auto expected = Volume<int>{{0,0},2048,2048};
  REQUIRE(tree.node_at_depth(0, 0).val.vol == expected);

  // level 1
  expected = Volume<int>{{0,0}, 1024, 1024};
  REQUIRE(tree.node_at_depth(1, 0).val.vol == expected);
  expected = Volume<int>{{1024,0}, 1024, 1024};
  REQUIRE(tree.node_at_depth(1, 1).val.vol == expected);
  expected = Volume<int>{{0,1024}, 1024, 1024};
  REQUIRE(tree.node_at_depth(1, 2).val.vol == expected);
  expected = Volume<int>{{1024,1024}, 1024, 1024};
  REQUIRE(tree.node_at_depth(1, 3).val.vol == expected);

  // level 2.0
  expected = Volume<int>{{0,0}, 512, 512};
  REQUIRE(tree.node_at_depth(2, 0).val.vol == expected);
  expected = Volume<int>{{512,0}, 512, 512};
  REQUIRE(tree.node_at_depth(2, 1).val.vol == expected);
  expected = Volume<int>{{0,512}, 512, 512};
  REQUIRE(tree.node_at_depth(2, 2).val.vol == expected);
  expected = Volume<int>{{512,512}, 512, 512};
  REQUIRE(tree.node_at_depth(2, 3).val.vol == expected);

  // level 2.1
  expected = Volume<int>{{1024, 0}, 512, 512};
  REQUIRE(tree.node_at_depth(2, 4).val.vol == expected);

  expected = Volume<int>{{1024 + 512, 0}, 512, 512};
  REQUIRE(tree.node_at_depth(2, 5).val.vol == expected);

  expected = Volume<int>{{1024, 512}, 512, 512};
  REQUIRE(tree.node_at_depth(2, 6).val.vol == expected);

  expected = Volume<int>{{1024 + 512, 512}, 512, 512};
  REQUIRE(tree.node_at_depth(2, 7).val.vol == expected);

  // level 2.2
  expected = Volume<int>{{0, 1024}, 512, 512};
  REQUIRE(tree.node_at_depth(2, 8).val.vol == expected);

  expected = Volume<int>{{512, 1024}, 512, 512};
  REQUIRE(tree.node_at_depth(2, 9).val.vol == expected);

  expected = Volume<int>{{0, 1024 + 512}, 512, 512};
  REQUIRE(tree.node_at_depth(2, 10).val.vol == expected);

  expected = Volume<int>{{512, 1024 + 512}, 512, 512};
  REQUIRE(tree.node_at_depth(2, 11).val.vol == expected);

  // level 2.3
  expected = Volume<int>{{1024, 1024}, 512, 512};
  REQUIRE(tree.node_at_depth(2, 12).val.vol == expected);

  expected = Volume<int>{{1024 + 512, 1024}, 512, 512};
  REQUIRE(tree.node_at_depth(2, 13).val.vol == expected);

  expected = Volume<int>{{1024, 1024 + 512}, 512, 512};
  REQUIRE(tree.node_at_depth(2, 14).val.vol == expected);

  expected = Volume<int>{{1024 + 512, 1024 + 512}, 512, 512};
  REQUIRE(tree.node_at_depth(2, 15).val.vol == expected);
}
