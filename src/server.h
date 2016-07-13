/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "common/reactor.h"
#include "event.h"
namespace redc
{
  struct Player;

  struct Server_Base : public Event_Sink<Event>
  {
    virtual ~Server_Base() {}

    // Request a player to be created and spawned in, accessible after the
    // event has been made through Server_Base::player.
    virtual void req_player() = 0;
    virtual Player& player(player_id id) = 0;

    virtual void load_map(std::string const& map) = 0;
  };
}
