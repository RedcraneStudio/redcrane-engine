/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "../collisionlib/sweep_and_prune.h"

#include "catch/catch.hpp"

TEST_CASE("SAP algorithm detects AABB collision", "[SAP]")
{
  using namespace game;

  SAP sap{};

  AABB a1;

  a1.min = glm::vec3(0.0f, 0.0f, 0.0f);
  a1.width = 50.0f;
  a1.height = 50.0f;
  a1.depth = 50.0f;

  AABB a2 = a1;
  a2.min = glm::vec3(25.0f, 25.0f, 25.0f);

  AABB a3 = a2;
  a3.min = glm::vec3(55.0f, 55.0f, 55.0f);

  AABB a4 = a2;
  a4.min = glm::vec3(2.0f, 2.0f, 2.0f);

  //--------------------
  //        a1         -
  //--------------------
  //         --------------------
  //         -        a2        -
  //         --------------------
  //                          --------------------
  //                          -        a3        -
  //                          --------------------
  //  --------------------
  //  -        a4        -
  //  --------------------
  //
  // Imagine y axis compressed so the four aabbs are mostly intersecting (see
  // below).
  //
  // Expected pairs: (Not necessarily in that order)
  // (1, 2), (1, 4), (2, 4), (2, 3).

  auto id1 = sap.insert(a1);
  auto id2 = sap.insert(a2);
  auto id3 = sap.insert(a3);
  auto id4 = sap.insert(a4);

  auto result = sap.collisions();

  // Checks equality between pairs, but not necessarily in the same order.
  // As long as both pairs *contain* the same ids (or whatever) they will be
  // said to be equal, with this function.
  auto pairs_equal = [](auto const& fp, auto const& sp)
  {
    return (std::get<0>(fp) == std::get<0>(sp) &&
            std::get<1>(fp) == std::get<1>(sp)) ||
           (std::get<0>(fp) == std::get<1>(sp) &&
            std::get<1>(fp) == std::get<0>(sp));
  };

  REQUIRE(result.size() == 4);

  // We shouldn't depend on the order but for now this is just how it's going
  // to go :)
  REQUIRE(pairs_equal(result[0], std::make_pair(id1, id4)));
  REQUIRE(pairs_equal(result[1], std::make_pair(id1, id2)));
  REQUIRE(pairs_equal(result[2], std::make_pair(id4, id2)));
  REQUIRE(pairs_equal(result[3], std::make_pair(id2, id3)));
}
