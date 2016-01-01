/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "player_data.h"
namespace redc
{
  Game_State& get_state() noexcept
  {
    static Game_State state;
    return state;
  }
}
