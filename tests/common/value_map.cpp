/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */

#include "catch/catch.hpp"

#include "common/value_map.h"

TEST_CASE("value map sampling works", "[value_map]")
{
  using namespace redc;

  SECTION("integer sampling works as expected")
  {
    Value_Map<int> map;
    map.allocate({100,100});

    SECTION("sample of one location returns that value")
    {
      // Basically check to make sure basic sampling of a single point returns
      // that point. Very important!!
      int expected = 0;
      Vec<size_t> pos;

      SECTION("one")
      {
        expected = 3;
        pos = {5,5};
      }
      SECTION("two")
      {
        expected = 7;
        pos = {0,0};
      }
      SECTION("three")
      {
        expected = 1;
        pos = {100,100};
      }
      SECTION("four")
      {
        expected = 4;
        pos = {75,75};
      }

      map.at(pos) = expected;
      // This should be a given, but it won't hurt I guess. I'm still not sure
      // exactly what to test where I guess. I mean, obviously this should be in
      // a test of it's own.
      CHECK(map.at(pos) == expected);

      REQUIRE(sample(map, {pos,1,1}) == expected);
    }
    SECTION("sample of larger area returns (integer) average")
    {
      SECTION("single value average")
      {
        int expected = 4;

        // Fill an 2x2 area.
        map.at({0,0}) = expected;
        map.at({0,1}) = expected;
        map.at({1,0}) = expected;
        map.at({1,1}) = expected;
        REQUIRE(sample(map, {{0, 0}, 2, 2}) == expected);

        // Fill an 3x3 area.
        expected = 7;
        map.at({10,70}) = expected;
        map.at({10,71}) = expected;
        map.at({10,72}) = expected;
        map.at({11,70}) = expected;
        map.at({11,71}) = expected;
        map.at({11,72}) = expected;
        map.at({12,70}) = expected;
        map.at({12,71}) = expected;
        map.at({12,72}) = expected;
        REQUIRE(sample(map, {{10, 70}, 3, 3}) == expected);
      }
      SECTION("multiple-value average")
      {
        // We are only going to deal with numbers with a perfectly integer
        // average (that is, without truncation being necessary).

        // In this case we have a lot of different values so an expected var is
        // unnecessary.

        // Fill an 2x2 area.
        map.at({0,0}) = 6;
        map.at({0,1}) = 10;
        map.at({1,0}) = 8;
        map.at({1,1}) = 8;
        REQUIRE(sample(map, {{0, 0}, 2, 2}) == 8);

        // Fill an 3x3 area.
        map.at({10,70}) = 5;
        map.at({10,71}) = 12;
        map.at({10,72}) = 27;
        map.at({11,70}) = 3;
        map.at({11,71}) = 1;
        map.at({11,72}) = 7;
        map.at({12,70}) = 60;
        map.at({12,71}) = 27;
        map.at({12,72}) = 2;
        REQUIRE(sample(map, {{10, 70}, 3, 3}) == 16);
      }
    }
  }
  SECTION("floating-point sampling works as expected")
  {
    Value_Map<float> map;
    map.allocate({100,100});

    float expected = 8.2456;
    map.at({65,65}) = expected;

    // Can we do an equality comparison here?
    REQUIRE(sample(map, {{65,65},1,1}) == Approx(expected));

    map.at({5,6}) = 5.5;
    map.at({5,7}) = 6.5;
    map.at({6,6}) = 6.5;
    map.at({6,7}) = 7.5;
    REQUIRE(sample(map, {{5,6}, 2, 2}) == Approx(6.5));

    map.at({20,50}) = 51.23;
    map.at({20,51}) = 50.124;
    map.at({20,52}) = 2.941;
    map.at({21,50}) = 1.789;
    map.at({21,51}) = 5.66;
    map.at({21,52}) = 9.995;
    map.at({22,50}) = 241.5;
    map.at({22,51}) = 3.34;
    map.at({22,52}) = 4.85;
    REQUIRE(sample(map, {{20, 50}, 3, 3}) == Approx(41.26989));
  }
}
