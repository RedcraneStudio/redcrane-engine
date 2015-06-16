/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "../common/maybe_owned.hpp"

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

using namespace game;

TEST_CASE("Constructed to null", "[Maybe_Owned]")
{
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
TEST_CASE("Destruction when owned", "[Maybe_Owned]")
{
  struct Is_Destructed
  {
    Is_Destructed(bool* is) noexcept : is(is) { *is = false; }
    ~Is_Destructed() noexcept { *is = true; }

    bool* is;
  };

  bool is_destructed;
  {
    auto ptr = Maybe_Owned<Is_Destructed>{&is_destructed};
  }
  REQUIRE(is_destructed == true);

  auto destructed = std::make_unique<Is_Destructed>(&is_destructed);
  {
    // Construct a maybe owned that *doesn't* own it's ptr.
    auto ptr = Maybe_Owned<Is_Destructed>{destructed.get()};
  }
  REQUIRE(is_destructed == false);

}
