/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include <vector>
namespace game
{
  struct Player
  {
    std::string name;
  };

  struct Game_State
  {
    std::vector<Player> players;
  };

  Game_State& get_state() noexcept;
}
