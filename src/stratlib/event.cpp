/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "event.h"

#include <algorithm>

#include "../common/debugging.h"
#include "../common/algorithm.h"
namespace game { namespace strat
{
  template <class Container, class T>
  static bool is_contained(Container& container, T const& value) noexcept
  {
    using std::begin; using std::end;
    return std::binary_search(begin(container), end(container), value);
  };

  template <class Container, class T>
  void lower_bound_insert(Container& container, T const& value) noexcept
  {
    using std::begin; using std::end;
    auto iter = std::lower_bound(begin(container), end(container), value);
    if(iter != end(container))
    {
      if(*iter != value)
      {
        container.insert(iter, value);
      }
    }
    else container.insert(iter, value);
  }

  // 2 2 2 2 2 3 3 3 4
  // 2 1 1 1 1 3 3 3 4
  // 2 1 1 1 1 3 3 3 4
  // 2 1 1 1 1 3 3 3 4
  // 2 4 4 4 4 4 4 4 4
  //                 ^
  void spread(Cost_Map const& cost, Event_Map& event_map) noexcept
  {
    // The size shouldn't change even if we push some into the queue.
    auto old_active_events = std::move(event_map.active_events);

    // This is implied?
    event_map.active_events.clear();

    for(auto pos : old_active_events)
    {
      // This current position has been visited.
      lower_bound_insert(event_map.visited_events, pos);

      // Factor in timings

      // Add surrounding positions.

      // Left
      --pos.x;
      // If left hasn't been visited: add it
      if(!is_contained(event_map.visited_events, pos))
      {
        event_map.active_events.push_back(pos);
      }

      // Right
      ++pos.x; ++pos.x;
      // See above comment.
      if(!is_contained(event_map.visited_events, pos))
      {
        event_map.active_events.push_back(pos);
      }

      // Down
      --pos.x; --pos.y;
      if(!is_contained(event_map.visited_events, pos))
      {
        event_map.active_events.push_back(pos);
      }

      // Above
      ++pos.y; ++pos.y;
      if(!is_contained(event_map.visited_events, pos))
      {
        event_map.active_events.push_back(pos);
      }
    }
  }
} }
