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
  template <class T>
  static bool is_in_bounds_of(Value_Map<T> const& vm, Vec<int> pos) noexcept
  {
    return 0 <= pos.x && pos.x < vm.extents.x &&
           0 <= pos.y && pos.y < vm.extents.y;
  };

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
    GAME_ASSERT(cost.extents == event_map.visited_map.extents);

    // The size shouldn't change even if we push some into the queue.
    auto old_active_events = std::move(event_map.active_events);

    // This is implied?
    event_map.active_events.clear();

    for(auto pos : old_active_events)
    {
      // This current position has been visited.
      event_map.visited_map.at(pos) = true;

      // Factor in timings

      // Add surrounding positions.

      // Left
      --pos.x;
      // If left hasn't been visited: add it
      if(!event_map.visited_map.at(pos) &&
         is_in_bounds_of(event_map.visited_map, pos) &&
         !is_contained(event_map.active_events, pos))
      {
        lower_bound_insert(event_map.active_events, pos);
      }

      // Right
      ++pos.x; ++pos.x;
      // See above comment.
      if(!event_map.visited_map.at(pos) &&
         is_in_bounds_of(event_map.visited_map, pos) &&
         !is_contained(event_map.active_events, pos))
      {
        lower_bound_insert(event_map.active_events, pos);
      }

      // Down
      --pos.x; --pos.y;
      if(!event_map.visited_map.at(pos) &&
         is_in_bounds_of(event_map.visited_map, pos) &&
         !is_contained(event_map.active_events, pos))
      {
        lower_bound_insert(event_map.active_events, pos);
      }

      // Above
      ++pos.y; ++pos.y;
      if(!event_map.visited_map.at(pos) &&
         is_in_bounds_of(event_map.visited_map, pos) &&
         !is_contained(event_map.active_events, pos))
      {
        lower_bound_insert(event_map.active_events, pos);
      }
    }
  }
} }
