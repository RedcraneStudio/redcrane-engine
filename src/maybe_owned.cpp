/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "maybe_owned.hpp"
#include "catch/catch.hpp"

TEST_CASE("Constructed to null", "[Maybe_Owned]")
{
  using namespace survive;

  Maybe_Owned<int> i;
  REQUIRE(i.is_owned() == false);
  REQUIRE(i.is_pointer() == true);
  REQUIRE(i.get() == nullptr);
  REQUIRE(static_cast<bool>(i) == false);
  REQUIRE(!static_cast<bool>(i) == true);

  SECTION("Can own")
  {
    i.set_owned(int{5});
    REQUIRE(i.is_owned() == true);

    SECTION("Value is persistent")
    {
      REQUIRE(*i == 5);
      REQUIRE(*i.get() == 5);
    }
    SECTION("Dereference and get are equivalent")
    {
      REQUIRE(&(*i) == i.get());
      REQUIRE(*i == *i.get());
    }
  }
}
