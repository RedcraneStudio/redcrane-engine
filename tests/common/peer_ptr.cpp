/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "catch/catch.hpp"
#include "common/peer_ptr.hpp"

TEST_CASE("Peer pointer and locking works", "[Peer_Ptr]")
{
  auto peer_ptrs = redc::make_peer_ptrs<int>();

  // Make sure they own the same pointer
  CHECK(peer_ptrs.first.get() == peer_ptrs.second.get());

  // Make sure changes propagate to both (should be a given).
  *peer_ptrs.first = 5;
  CHECK(5 == *peer_ptrs.first);
  CHECK(5 == *peer_ptrs.second);

  SECTION("No locking")
  {
    // And when one goes out of scope
    {
      auto second_ptr = std::move(peer_ptrs.second);

      // Still valid...
      CHECK(peer_ptrs.first.get() == second_ptr.get());
      CHECK(peer_ptrs.first.get() != nullptr);

      CHECK(peer_ptrs.second.get() == nullptr);

      // Dead
    }

    CHECK(peer_ptrs.first.get() == nullptr);
    CHECK(peer_ptrs.second.get() == nullptr);
  }
  SECTION("Locking")
  {
    {
      // Lock the resource
      auto lock = peer_ptrs.first.lock();

      // Is the lock reasonable?
      CHECK(lock.get() == peer_ptrs.first.get());
      CHECK(lock.get() == peer_ptrs.second.get());
      {
        // Now move a peer out of the pair and let it be destructed.
        auto peer = std::move(peer_ptrs.first);
        CHECK(peer.get() == peer_ptrs.second.get());

        // The one we moved from should be a nullptr;
        CHECK(peer_ptrs.first.get() == nullptr);
      }

      // The data should still be valid here, because we still have an active
      // lock.
      CHECK(lock.get() != nullptr);
      CHECK(lock.get() == peer_ptrs.second.get());
    }
    // But not valid here
    CHECK(peer_ptrs.second.get() == nullptr);

    // Unfortunately we can't verify that the data container has been properly
    // deallocated, that's a test for a valgrind.
  }
}
