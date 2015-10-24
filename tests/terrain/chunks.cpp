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

TEST_CASE("correct vertex count for terrain grid", "[terrainlib]")
{
  using namespace game::terrain;
  REQUIRE(detail::mesh_vertices(0, 1, 3) == 54);
  REQUIRE(detail::mesh_vertices(1, 2, 3) == 216);
  REQUIRE(detail::mesh_vertices(0, 2, 3) == 270);
  REQUIRE(detail::mesh_vertices(2, 3, 3) == 864);
  REQUIRE(detail::mesh_vertices(0, 3, 3) == 270 + 864);

  REQUIRE(detail::mesh_vertices(0, 1, 5) == 150);
  REQUIRE(detail::mesh_vertices(1, 2, 5) == 600);
  REQUIRE(detail::mesh_vertices(0, 2, 5) == 750);
  REQUIRE(detail::mesh_vertices(2, 3, 5) == 2400);
  REQUIRE(detail::mesh_vertices(0, 3, 5) == 750 + 2400);
}
TEST_CASE("physical size distribution works", "[terrainlib]")
{
  using namespace game;

  terrain::terrain_tree_t tree;

  tree.set_depth(3);

  terrain::set_physical_size(tree, {600.0f, 1200.0f});

  REQUIRE(tree.node_at_depth(0,0).val.physical_size.x == Approx(600.0));
  REQUIRE(tree.node_at_depth(0,0).val.physical_size.y == Approx(1200.0));

  for(auto iter = tree.level_begin(1); iter < tree.level_end(1); ++iter)
  {
    REQUIRE(iter->val.physical_size.x == Approx(300.0));
    REQUIRE(iter->val.physical_size.y == Approx(600.0));
  }

  for(auto iter = tree.level_begin(2); iter < tree.level_end(2); ++iter)
  {
    REQUIRE(iter->val.physical_size.x == Approx(150.0));
    REQUIRE(iter->val.physical_size.y == Approx(300.0));
  }
}
