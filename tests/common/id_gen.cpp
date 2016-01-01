/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "catch/catch.hpp"
#include "common/id_gen.hpp"

TEST_CASE("Uses all available ids", "[idgen]")
{
  using redc::ID_Gen;

  using id_type = int16_t;

  ID_Gen<id_type> counter;

  for(id_type id = 0; ++id != 0;)
  {
    // We need to exhaust all the ids available.
    counter.get();
  }

  // We should have no ids left.
  CHECK(0 == counter.get());

  // Make room.
  counter.remove(1);

  // We should have at least room for one more.
  CHECK_FALSE(0 == counter.get());
}
