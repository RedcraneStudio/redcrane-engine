/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <unordered_map>
#include "../boatlib/boat.h"
#include "../collisionlib/motion.h"
namespace redc { namespace sail
{
  struct Player
  {
    std::string name;
    bool spawned;

    Hull_Desc boat_config;
    collis::Motion boat_motion;

    // For server methods: this is the Network_Client* that owns the player.
    void* userdata;
  };

  using player_id = uint16_t;
  struct Game
  {
    std::unordered_map<player_id, Player> players;
  };
} }
