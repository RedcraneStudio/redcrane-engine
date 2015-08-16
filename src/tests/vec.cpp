/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#include "../common/vec.h"

#include "catch/catch.hpp"
namespace game
{
  TEST_CASE("Project onto point axis works", "[Vec]")
  {
    using namespace game;

    auto initial_pt = Vec<float>{0.0f, 0.0f};

    auto res = Vec<float>{};
    auto expected = Vec<float>{};

    res = project_onto_pt_axes(initial_pt, {5.0f, 0.0f});
    expected = Vec<float>{5.0f, 0.0f};
    REQUIRE(res == expected);

    res = project_onto_pt_axes(initial_pt, {5.0f, 2.0f});
    expected = Vec<float>{5.0f, 0.0f};
    REQUIRE(res == expected);

    res = project_onto_pt_axes(initial_pt, {2.0f, 5.0f});
    expected = Vec<float>{0.0f, 5.0f};
    REQUIRE(res == expected);

    res = project_onto_pt_axes(initial_pt, {2.0f, -5.0f});
    expected = Vec<float>{0.0f, -5.0f};
    REQUIRE(res == expected);
  }
}
