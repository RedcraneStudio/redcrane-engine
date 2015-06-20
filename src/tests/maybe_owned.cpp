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
TEST_CASE("Pointer is destructed properly", "[Maybe_Owned]")
{
  struct Is_Destructed
  {
    Is_Destructed(bool* is) noexcept : is(is) { *is = false; }
    ~Is_Destructed() noexcept { *is = true; }

    bool* is;
  };

  SECTION("Owning maybe destructs pointer; Pointing maybe doesn't.")
  {
    bool is_destructed;
    {
      auto ptr = Maybe_Owned<Is_Destructed>{&is_destructed};
    }
    REQUIRE(is_destructed == true);

    auto destructed = std::make_unique<Is_Destructed>(&is_destructed);
    {
      // Construct a maybe owned that *doesn't* own it's ptr.
      auto ptr = Maybe_Owned<Is_Destructed>{destructed.get(), false};
    }
    REQUIRE(is_destructed == false);
  }

  SECTION("Won't move into itself to cause destruction")
  {
    bool is_destructed;

    // First maybe owned is owned.
    auto fmo = make_maybe_owned<Is_Destructed>(&is_destructed);
    REQUIRE(fmo.is_owned());

    {
      // The second is now owning whatever fmo had.
      auto smo = std::move(fmo);
      REQUIRE(smo.is_owned());

      // The pointer should be the same, but now we are moving from a maybe that
      // doesn't own it's data.
      smo = std::move(fmo);

      // It shouldn't matter!
      REQUIRE(smo.is_owned());
    }

    // Finally, make sure our thing was destructed.
    REQUIRE(is_destructed);
  }
}
TEST_CASE("Move won't nullify pointer", "[Maybe_Owned]")
{
  // Construct a regular owned maybe.
  auto mo = make_maybe_owned<int>(5);
  REQUIRE(*mo == 5); // It must be equal to five.
  REQUIRE(mo.is_owned()); // It must be owned.

  // Move it somewhere else.
  auto other = std::move(mo);

  // It better be owned by other.
  REQUIRE(other.is_owned());

  // And mo should be pointing now.
  REQUIRE(mo.is_pointer());

  // Most importantly, mo must still be equal to five.
  REQUIRE(*mo == 5);
  // On that note, so should other.
  REQUIRE(*other == 5);

  // Therefore they are equal
  REQUIRE(*mo == *other);
  // And point to the same thing.
  REQUIRE(mo.get() == other.get());
}
