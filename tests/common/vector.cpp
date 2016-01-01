/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "catch/catch.hpp"
#include "common/vec.h"
#include <cstdint>

TEST_CASE("Vector equality works", "[vec]")
{
  redc::Vec<uint16_t> vecui(5, 5);
  CHECK(vecui == vecui);

  redc::Vec<double> vecd(5.5, 5.5);
  CHECK(vecd == vecd);
}
TEST_CASE("Vector rotation works", "[vec]")
{
  auto calc = redc::rotate(redc::Vec<double>(1,0), 3.14159);
  redc::Vec<double> expected(-1, 0);

  CHECK(expected.x == Approx(calc.x));
  CHECK(expected.y == Approx(calc.y));
}
