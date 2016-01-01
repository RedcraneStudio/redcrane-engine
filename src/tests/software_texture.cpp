/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */

#include "../gfx/support/software_texture.h"

#include "catch/catch.hpp"

TEST_CASE("Software texture works as expected", "[struct Software_Texture]")
{
  using namespace redc;

  Software_Texture texture;
  texture.allocate(Vec<int>{5, 5});

  SECTION("Blitting data and access works as expected")
  {
    texture.blit_data(Volume<int>{{0,0}, 1, 1}, &colors::green);
    REQUIRE(texture.get_data()[0] == colors::green);
    REQUIRE(texture.get_row(0)[0] == colors::green);
    REQUIRE(texture.get_pt(Vec<int>{0,0}) == colors::green);
  }
}
