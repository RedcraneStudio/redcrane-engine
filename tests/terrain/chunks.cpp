/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */

#include <string>
#include "common/volume.h"

#include "catch/catch.hpp"

#include "terrain/chunks.h"

#include "gfx/null/driver.h"

void compare_volumes(game::terrain::terrain_tree_t& tree, std::size_t level,
                     std::size_t index, game::Volume<int> uv_vol,
                     game::Volume<float> world_vol) noexcept
{
  auto& node = tree.node_at_depth(level,index);
  REQUIRE(node.val.uv_vol == uv_vol);

  REQUIRE(node.val.world_vol.pos.x == Approx(world_vol.pos.x));
  REQUIRE(node.val.world_vol.pos.y == Approx(world_vol.pos.y));
  REQUIRE(node.val.world_vol.width == Approx(world_vol.width));
  REQUIRE(node.val.world_vol.height == Approx(world_vol.height));
};

TEST_CASE("set_volumes works as expected", "[terrainlib]")
{
  using namespace game;

  Quadtree<terrain::Chunk> tree;
  tree.set_depth(3);
  terrain::set_volumes(tree, Vec<float>{1024.0, 1024.0f}, Vec<int>{2048,2048});

  compare_volumes(tree, 0,0,
                  {{0,0}, 2048, 2048},
                  {{0.0f, 0.0f}, 1024.0f, 1024.0f});

  // level 1
  compare_volumes(tree, 1, 0,
                  {{0, 0}, 1024, 1024},
                  {{0.0f, 0.0f}, 512.0f, 512.0f});

  compare_volumes(tree, 1, 1,
                  {{1024, 0}, 1024, 1024},
                  {{512.0f, 0.0f}, 512.0f, 512.0f});

  compare_volumes(tree, 1, 2,
                  {{0, 1024}, 1024, 1024},
                  {{0.0f, 512.0f}, 512.0f, 512.0f});

  compare_volumes(tree, 1, 3,
                  {{1024, 1024}, 1024, 1024},
                  {{512.0f, 512.0f}, 512.0f, 512.0f});

  // level 2.0
  compare_volumes(tree, 2, 0,
                  {{0, 0}, 512, 512},
                  {{0.0f, 0.0f}, 256.0f, 256.0f});

  compare_volumes(tree, 2, 1,
                  {{512, 0}, 512, 512},
                  {{256.0f, 0.0f}, 256.0f, 256.0f});

  compare_volumes(tree, 2, 2,
                  {{0, 512}, 512, 512},
                  {{0.0f, 256.0f}, 256.0f, 256.0f});

  compare_volumes(tree, 2, 3,
                  {{512, 512}, 512, 512},
                  {{256.0f, 256.0f}, 256.0f, 256.0f});

  // level 2.1
  compare_volumes(tree, 2, 4,
                  {{1024, 0}, 512, 512},
                  {{512.0f, 0.0f}, 256.0f, 256.0f});

  compare_volumes(tree, 2, 5,
                  {{1024 + 512, 0}, 512, 512},
                  {{512.0f + 256.0f, 0.0f}, 256.0f, 256.0f});

  compare_volumes(tree, 2, 6,
                  {{1024, 512}, 512, 512},
                  {{512.0f, 256.0f}, 256.0f, 256.0f});

  compare_volumes(tree, 2, 7,
                  {{1024 + 512, 512}, 512, 512},
                  {{512.0f + 256.0f, 256.0f}, 256.0f, 256.0f});

  // level 2.2
  compare_volumes(tree, 2, 8,
                  {{0, 1024}, 512, 512},
                  {{0.0f, 512.0f}, 256.0f, 256.0f});

  compare_volumes(tree, 2, 9,
                  {{512, 1024}, 512, 512},
                  {{256.0f, 512.0f}, 256.0f, 256.0f});

  compare_volumes(tree, 2, 10,
                  {{0, 1024 + 512}, 512, 512},
                  {{0.0f, 512.0f + 256.0f}, 256.0f, 256.0f});

  compare_volumes(tree, 2, 11,
                  {{512, 1024 + 512}, 512, 512},
                  {{256.0f, 512.0f + 256.0f}, 256.0f, 256.0f});

  // level 2.3
  compare_volumes(tree, 2, 12,
                  {{1024, 1024}, 512, 512},
                  {{512.0f, 512.0f}, 256.0f, 256.0f});

  compare_volumes(tree, 2, 13,
                  {{1024 + 512, 1024}, 512, 512},
                  {{512.0f + 256.0f, 512.0f}, 256.0f, 256.0f});

  compare_volumes(tree, 2, 14,
                  {{1024, 1024 + 512}, 512, 512},
                  {{512.0f, 512.0f + 256.0f}, 256.0f, 256.0f});

  compare_volumes(tree, 2, 15,
                  {{1024 + 512, 1024 + 512}, 512, 512},
                  {{512.0f + 256.0f, 512.0f + 256.0f}, 256.0f, 256.0f});
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
TEST_CASE("initialize_vertices works as expected", "[terrainlib]")
{
  using namespace game;
  using namespace game::terrain;

  terrain_tree_t tree;

  // Initialize the tree for the vertices.
  tree.set_depth(3);

  // The heightmap is 20 by 20 and the world is 10 by 10
  set_volumes(tree, {10.0f, 10.0f}, {20,20});

  gfx::null::Driver null_driver{{1000,1000}};

  // Only set vertices for the last level in this tree with three levels. A
  // grid is 5x5 of vertices.
  initialize_vertices(tree, null_driver, 2, 5);
}
