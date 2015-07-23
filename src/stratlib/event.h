/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <list>
#include <vector>
#include "value_map.h"
namespace game { namespace strat
{
  struct Event_Payload {};

  struct Event
  {
    // Has been processed? For the flood fill.
    Event_Payload* payload;
  };

  // The value is the amount of time in seconds it takes for an event to move.
  using Cost_Map = Value_Map<float>;

  struct Event_Map
  {
    std::vector<Vec<int>> visited_events;
    std::vector<Vec<int>> active_events;
  };

  void spread(Cost_Map const&, Event_Map&) noexcept;
} }
