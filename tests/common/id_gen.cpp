/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "catch/catch.hpp"
#include "common/id_gen.hpp"

TEST_CASE("Uses all available ids", "[idgen]")
{
  using redc::ID_Gen;

  using id_type = uint8_t;

  ID_Gen<id_type> counter;

  // Make sure no number is valid at all
  for(id_type id = 0; ++id != 0;)
  {
    CHECK(counter.is_valid(id) == false);
  }

  for(id_type id = 0; ++id != 0;)
  {
    // We need to exhaust all the ids available.
    counter.get();
  }

  // We should have no ids left.
  CHECK(0 == counter.get());

  // Exhaust some more!!!
  for(int i = 0; i < 100; ++i) counter.get();

  // Make room.
  counter.remove(1);

  // We should have at least room for one more.
  CHECK_FALSE(0 == counter.get());

  // Make sure what has been reserved everything
  CHECK(std::numeric_limits<id_type>::max() == counter.reserved());

  // These ids should be valid
  CHECK(counter.is_valid(5));
  CHECK(counter.is_valid(10));

  // Zero is not valid
  CHECK_FALSE(counter.is_valid(0));

  counter.remove(5);
  counter.remove(9);

  CHECK_FALSE(counter.is_valid(5));
  CHECK(counter.is_removed(5));

  CHECK_FALSE(counter.is_valid(9));
  CHECK(counter.is_removed(9));

  CHECK_FALSE(counter.is_valid(0));

  // We just removed two so we should now have two less reserved than we had
  // before
  CHECK(std::numeric_limits<id_type>::max() - 2 == counter.reserved());
}
TEST_CASE("Peeking gives next id", "[idgen]")
{
  using redc::ID_Gen;

  // This code should work regardless of size of id.
  using id_type = uint16_t;

  ID_Gen<id_type> counter;
  for(id_type i = 0; ++i != 0;)
  {
    auto expected_next = counter.peek();
    auto next = counter.get();
    CHECK(expected_next == next);
  }

  // What happens once we don't have any more ids.
  auto expected = counter.peek();
  auto got = counter.get();
  CHECK(0 == expected);
  CHECK(0 == got);
}
