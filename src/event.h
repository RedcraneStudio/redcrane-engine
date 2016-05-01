/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <cstdint>
#include <boost/variant.hpp>
#include "common/observer_ptr.hpp"
namespace redc
{
  using player_id = uint16_t;
  struct New_Player_Event
  {
    player_id id;
    bool owned;
  };

  struct Map;
  struct Map_Loaded_Event
  {
    observer_ptr<Map> map;
  };

  using Event = boost::variant<New_Player_Event, Map_Loaded_Event>;
}
