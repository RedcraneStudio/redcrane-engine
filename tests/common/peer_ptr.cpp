/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "catch/catch.hpp"
#include "common/peer_ptr.hpp"

TEST_CASE("Peer pointer works", "[Peer_Ptr]")
{
  auto peer_ptrs = redc::make_peer_ptrs<int>();

  // Make sure they own the same pointer
  CHECK(peer_ptrs.first.get() == peer_ptrs.second.get());

  // Make sure changes propagate to both (should be a given).
  *peer_ptrs.first = 5;
  CHECK(5 == *peer_ptrs.first);
  CHECK(5 == *peer_ptrs.second);

  // And when one goes out of scope
  {
    auto second_ptr = std::move(peer_ptrs.second);

    // Still valid...
    CHECK(peer_ptrs.first.get() == second_ptr.get());
    CHECK(peer_ptrs.first.get() != nullptr);

    // Dead
  }

  CHECK(peer_ptrs.first.get() == nullptr);
  CHECK(peer_ptrs.second.get() == nullptr);
}
