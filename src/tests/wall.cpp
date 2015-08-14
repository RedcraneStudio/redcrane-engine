/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "../stratlib/wall.h"

#include "catch/catch.hpp"
namespace game
{
  TEST_CASE("Pending Wall end pt snapping works", "[Wall]")
  {
    using namespace game;

    auto wall_type = Wall_Type{1, 1.0f};

    auto pending_wall = Pending_Wall{wall_type, Vec<float>{0.0f, 0.0f}};

    auto res = pending_wall_end_pt(pending_wall, {5.0f, 0.0f});
    auto expected = Vec<float>{5.0f, 0.0f};
    REQUIRE(res == expected);

    res = pending_wall_end_pt(pending_wall, {5.0f, 2.0f});
    expected = Vec<float>{5.0f, 0.0f};
    REQUIRE(res == expected);

    res = pending_wall_end_pt(pending_wall, {2.0f, 5.0f});
    expected = Vec<float>{0.0f, 5.0f};
    REQUIRE(res == expected);

    res = pending_wall_end_pt(pending_wall, {2.0f, -5.0f});
    expected = Vec<float>{0.0f, -5.0f};
    REQUIRE(res == expected);
  }
}
