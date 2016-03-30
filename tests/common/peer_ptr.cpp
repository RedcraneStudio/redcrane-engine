/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "catch/catch.hpp"
#include "common/peer_ptr.hpp"

TEST_CASE("Peer pointer and locking works", "[Peer_Ptr]")
{
  auto peer_ptrs = redc::make_peer_pair<int>();

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
TEST_CASE("Making peers works", "[Peer_Ptr]")
{
  auto peer = redc::make_peer_ptr<int>(8);

  CHECK(*peer == 8);

  SECTION("Making a new valid peer")
  {
    {
      auto new_peer = peer.peer();
      CHECK(new_peer.get() == peer.get());
      CHECK(*new_peer == *peer);
    }
    // A peer was deallocated, this means the peer data should no longer be
    // valid
    CHECK(peer.get() == nullptr);
  }
  SECTION("Making a valid peer and locks")
  {
    {
      auto lock = peer.lock();
      {
        auto new_peer = peer.peer();
      }
      // A peer has deallocated!
      // But we still have a lock!
      CHECK(lock.get() == peer.get());
      CHECK(*lock == *peer);
    }
    // Byebye lock
    CHECK(peer.get() == nullptr);
  }
  SECTION("locking via new peers")
  {
    redc::Peer_Lock<int> lock = peer.lock();
    {
      // Move the peer into here.
      auto new_peer = std::move(peer);
      auto other_peer = new_peer.peer();

      CHECK(other_peer.get() == new_peer.get());
    }

    // No peers alive (we moved out of top-level 'peer') - make sure the data
    // is still alive (because of the lock).

    CHECK(lock.get() != nullptr);
    CHECK(*lock == 8);

  }
}
