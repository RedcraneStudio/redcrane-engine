/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "event.h"
#include "../common/debugging.h"
namespace game { namespace strat
{
  Event_Map create_event(Vec<int> extents, Vec<int> event_orig) noexcept
  {
    Event_Map event_map;

    event_map.map.allocate(extents);

    auto& event_ptr = event_map.map.at(event_orig);
    event_ptr = Event{false, nullptr};
    event_map.queue.push(&event_ptr);

    return event_map;
  }

  // 2 2 2 2 2 3 3 3 4
  // 2 1 1 1 1 3 3 3 4
  // 2 1 1 1 1 3 3 3 4
  // 2 1 1 1 1 3 3 3 4
  // 2 4 4 4 4 4 4 4 4
  //                 ^
  void spread(Cost_Map const& cost, Event_Map& event) noexcept
  {
    // Assert pre-conditions.
    GAME_ASSERT(cost.extents == event.map.extents);

    for(int i = 0; i < event.map.extents.y; ++i)
    {
      for(int j = 0; j < event.map.extents.x; ++j)
      {
        // This specific event at this location has already been processed.
        if(event.map.values[i].has_spread) continue;

        auto event_to_process = event.queue.front();
        event.queue.pop();

        event_to_process->has_spread = true;

        // Add surrounding positions.

        auto pos = Vec<int>{i,j};

        // Left
        --pos.x;
        event.queue.push(&event.map.at(pos));

        // Right
        ++pos.x; ++pos.x;
        event.queue.push(&event.map.at(pos));

        // Down
        --pos.x; --pos.y;
        event.queue.push(&event.map.at(pos));

        // Above
        ++pos.y; ++pos.y;
        event.queue.push(&event.map.at(pos));
      }
    }
  }
} }
