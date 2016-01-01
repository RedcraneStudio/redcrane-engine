/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "event.h"

#include <algorithm>

#include "../common/debugging.h"
#include "../common/algorithm.h"
namespace redc { namespace strat
{
  Event_Map::Event_Map(Vec<int> map_extents) noexcept : accum_time(0.0f)
  {
    visited_map.allocate(map_extents);

    // We rely on the memset in Value_Map allocation to give every event timer
    // inactive to start.
    event_timer_map.allocate(map_extents);
  }

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

  void adjust_event_at(Vec<int> pos, Event_Map& event_map,
                       float adj_start_time, float adj_cost) noexcept
  {
    // Our new event timer has a given position; The start time is old start
    // time + old (adj. pos) cost because that will avoid adding any excess
    // that was included in dt. In other words, using cost gets us to the exact
    // cutoff point that caused the event timer at some adjacent position fire.

    auto visited = event_map.visited_map.at(pos);
    auto is_in_bounds = is_in_bounds_of(event_map.visited_map, pos);
    // If it is in the bounds of the visited map we can assume that it is also
    // in the bounds of the other map, since this function is only called from
    // spread which checks this asserts this exactly.

    if(!visited && is_in_bounds)
    {
      auto& event_timer = event_map.event_timer_map.at(pos);
      if(!event_timer.active)
      {
        event_timer.active = true;
        event_timer.start_time = adj_start_time + adj_cost;
      }
      else
      {
        // If there is already an event timer going on this position. We need
        // to check to see if we can do better. In other words, does going to
        // position through our previous cell mean we could get to this new
        // position faster?

        // TODO: Figure out if this is even necessary.
        // The cell adjacent to this one that made it active in the first place
        // seems like it would always have less of a cost. In fact, this is
        // probably more of a test as to whether our simulation is correct. If
        // it is, this else should never fire.

        event_timer.start_time = std::min(event_timer.start_time,
                                          adj_start_time + adj_cost);
      }
    }
  }

  // 2 2 2 2 2 3 3 3 4
  // 2 1 1 1 1 3 3 3 4
  // 2 1 1 1 1 3 3 3 4
  // 2 1 1 1 1 3 3 3 4
  // 2 4 4 4 4 4 4 4 4
  //                 ^
  bool spread(Cost_Map const& cost, Event_Map& event_map, float dt) noexcept
  {
    GAME_ASSERT(cost.extents == event_map.visited_map.extents);

    // This isn't necessarily since allocation is done in construction and
    // should provide the invariant that both maps have the same extents. Fuck
    // it though it can't hurt.
    GAME_ASSERT(cost.extents == event_map.event_timer_map.extents);

    event_map.accum_time += dt;

    bool did_spread = false;
    for(int i = 0; i < event_map.event_timer_map.extents.y; ++i)
    {
      for(int j = 0; j < event_map.event_timer_map.extents.x; ++j)
      {
        auto pos = Vec<int>{j,i};

        // In case the right amount of time has elapsed, we need this mutable
        // so we can make it inactive.
        auto& event_timer = event_map.event_timer_map.at(pos);
        if(!event_timer.active) continue;

        // We are currently at an active event timer.
        auto elapsed_time = event_map.accum_time - event_timer.start_time;

        auto cur_cost = cost.at(pos);
        if(elapsed_time > cur_cost)
        {
          // Our event spread time for this position has already elapsed.

          // The event timer at this position no longer needs to be active.
          event_timer.active = false;
          // And this position should be considered visited.
          event_map.visited_map.at(pos) = true;

          // Spread now to adjacent positions.

          --pos.x;
          adjust_event_at(pos, event_map, event_timer.start_time, cur_cost);

          ++pos.x; ++pos.x;
          adjust_event_at(pos, event_map, event_timer.start_time, cur_cost);

          --pos.x; --pos.y;
          adjust_event_at(pos, event_map, event_timer.start_time, cur_cost);

          ++pos.y; ++pos.y;
          adjust_event_at(pos, event_map, event_timer.start_time, cur_cost);

          // We did spread!
          did_spread = true;
        }
      }
    }

    // If we ended up spreading at all, there is a chance that there are event
    // timers that could spread immediately, but where not processed.
    // The function can therefore be run again with a delta time of 0.0. That
    // will process all of those potential event timers without causing any
    // simulation, obviously.
    return did_spread;
  }
} }
