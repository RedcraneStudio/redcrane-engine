/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <queue>
#include <boost/variant.hpp>
#include "player.h"
namespace redc
{
  using player_id = uint16_t;
  struct New_Player_Event
  {
    player_id id;
    bool owned;
  };

  using Server_Event = boost::variant<New_Player_Event>;

  struct Server_Base
  {
    virtual ~Server_Base() {}

    // Request a player to be created and spawned in, accessible after the
    // event has been made through Server_Base::player.
    virtual void req_player() = 0;
    virtual Player& player(player_id id) = 0;

    bool poll_event(Server_Event& event);

  protected:
    void push_outgoing_event(Server_Event const& event);
  private:

    std::queue<Server_Event> event_queue_;
  };
}
