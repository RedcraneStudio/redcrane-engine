/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */

#include "catch/catch.hpp"

#include "common/volume.h"

TEST_CASE("volume quad returns the proper quadrant", "[volume]")
{
  using namespace game;

  SECTION("simple square integer volume at the origin")
  {
    Volume<int> in_vol{{0,0}, 1000, 1000};
    Volume<int> expect;

    expect = {{0,0}, 500, 500};
    CHECK(vol_quad(in_vol, 0) == expect);

    expect = {{500,0}, 500, 500};
    CHECK(vol_quad(in_vol, 1) == expect);

    expect = {{0,500}, 500, 500};
    CHECK(vol_quad(in_vol, 2) == expect);

    expect = {{500,500}, 500, 500};
    CHECK(vol_quad(in_vol, 3) == expect);
  }

  SECTION("square integer volume not at the origin")
  {
    Volume<int> in_vol = {{25,50}, 250, 250};
    Volume<int> expect;

    expect = {{25,50}, 125, 125};
    CHECK(vol_quad(in_vol, 0) == expect);

    expect = {{150,50}, 125, 125};
    CHECK(vol_quad(in_vol, 1) == expect);

    expect = {{25,175}, 125, 125};
    CHECK(vol_quad(in_vol, 2) == expect);

    expect = {{150,175}, 125, 125};
    CHECK(vol_quad(in_vol, 3) == expect);
  }
  SECTION("non-square integer volume not at the origin")
  {
    Volume<int> in_vol = {{5,50}, 100, 150};
    Volume<int> expect;

    expect = {{5,50}, 50, 75};
    CHECK(vol_quad(in_vol, 0) == expect);

    expect = {{55,50}, 50, 75};
    CHECK(vol_quad(in_vol, 1) == expect);

    expect = {{5,125}, 50, 75};
    CHECK(vol_quad(in_vol, 2) == expect);

    expect = {{55,125}, 50, 75};
    CHECK(vol_quad(in_vol, 3) == expect);
  }

  SECTION("non-square floating-point volume not at the origin")
  {
    Volume<float> in_vol = {{10.5, 20.5}, 30.5, 40.5};
    Volume<float> expect;
    Volume<float> result;

    SECTION("top-left")
    {
      expect = {{10.5, 20.5}, 15.25, 20.25};
      result = vol_quad(in_vol, 0);
    }
    SECTION("top-right")
    {
      expect = {{25.75, 20.5}, 15.25, 20.25};
      result = vol_quad(in_vol, 1);
    }
    SECTION("bottom-left")
    {
      expect = {{10.5, 40.75}, 15.25, 20.25};
      result = vol_quad(in_vol, 2);
    }
    SECTION("bottom-right")
    {
      expect = {{25.75, 40.75}, 15.25, 20.25};
      result = vol_quad(in_vol, 3);
    }

    CHECK(result.pos.x == Approx(expect.pos.x));
    CHECK(result.pos.y == Approx(expect.pos.y));
    CHECK(result.width == Approx(expect.width));
    CHECK(result.height == Approx(expect.height));
  }
}
