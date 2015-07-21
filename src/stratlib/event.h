/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <queue>
#include <vector>
#include "value_map.h"
namespace game { namespace strat
{
  struct Event_Payload {};

  struct Event
  {
    // Has been processed? For the flood fill.
    bool has_spread = false;
    Event_Payload* payload;
  };

  // The value is the amount of time in seconds it takes for an event to move.
  using Cost_Map = Value_Map<float>;

  using Event_Value_Map = Value_Map<Event>;
  struct Event_Map
  {
    Event_Value_Map map;
    std::queue<Event*> queue;
  };

  Event_Map create_event(Vec<int> extents, Vec<int> event_orig) noexcept;

  // Pre-condition: Same extents.
  void spread(Cost_Map const&, Event_Map&) noexcept;
} }
