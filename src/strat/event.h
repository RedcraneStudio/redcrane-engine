/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 *
 * This file contains the interface to the 'event spreading' functionality. In
 * other words, the implementation of our strategy game's niche!
 */
#pragma once
#include <list>
#include <vector>
#include "../common/value_map.h"
namespace redc { namespace strat
{
  // An ________ is used to decide when an event is ready to spread out.
  // Event_Timer seems to fit here I guess.
  struct Event_Timer
  {
    // Start time is relative to the accumulated time. Subtracting the
    // accumulated time from this start time will yield a greater number than
    // the cost at some position if this event is ready to spread from that
    // position.
    float start_time;

    // start_time is only considered relevant when active is true.
    bool active;
  };

  // The value is the amount of time in seconds it takes for an event to move.
  using Cost_Map = Value_Map<float>;

  struct Event_Map
  {
    Event_Map(Vec<int> map_extents) noexcept;

    Value_Map<bool> visited_map;

    // The rest is basically private stuff, but I don't see a real reason to
    // restrict access in this case.

    // We've decided to use a value map here instead of a vector positions and
    // data because this will scale better and the other design (while
    // implementing timings) would require reinsertion of event timers just to
    // keep the vector sorted. This is kind of hard to manage, sort of a waste,
    // and probably will not scale. A value map seems to be scaling fairly well
    // and so far boasts a simpler design.
    // They were kept sorted so that we could search them using the delicious
    // binary search algorithm. However, having to change the start time while
    // the timer was in the vector meant a reinsertion. The confusing part, was
    // that start_time didn't actually have to be factored into the sort
    // because there would only be one timer per position. This invariant
    // seemed kind of confusing to begin with. In addition, the vector of
    // positions still seemed like it would begin to break down at a much
    // larger scale. At this point, the scale of the map is undeterminable so
    // it would be best not to commit to anything. The reason I think it will
    // break down is because before using a Value_Map for the visited cells,
    // looking up which cells that have been visited became the bottleneck,
    // subsequently, removing it made everything a whole lot faster.
    Value_Map<Event_Timer> event_timer_map;

    float accum_time;
  };

  // We actually don't even need to accept a delta time in the function, the
  // client code can just add it to accum_time itself and then call spread
  // until it returns false.

  // Return value: Think 'NEEDS MORE SPREAD'.
  bool spread(Cost_Map const&, Event_Map&, float dt) noexcept;
} }
