/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../common/id_map.hpp"
#include "boat.h"
#include "../collisionlib/motion.h"
namespace redc { namespace sail
{
  struct Player
  {
    std::string name;
    bool spawned;

    Hull_Desc boat_config;
    collis::Motion boat_motion;

    void* userdata;
  };

  struct Game
  {
    ID_Map<Player> players;
  };

  using player_id = ID_Map<Player>::id_type;
} }
