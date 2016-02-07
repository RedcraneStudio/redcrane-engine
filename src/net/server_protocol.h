/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "client_protocol.h"

#include "../common/id_map.hpp"
namespace redc { namespace net
{
  // Sent from server to client about all clients (even the owner).
  // We should optimize this packer since a client that knows its own inputs
  // has no need for this information, which currently is like 5.5 bytes, I
  // think?
  struct State
  {
    // The time of this state update is input.time.
    Input input;

    // We can't quantize the position, but we can do it for the velocity.
    // For now I'm not going to worry about it because it means some hacks with
    // message pack.

    // This is all about the boat.
    glm::vec3 position;
    glm::vec3 velocity;
    glm::quat angular_displacement;
    glm::quat angular_velocity;

    MSGPACK_DEFINE(input, position, velocity, angular_displacement,
                   angular_velocity);
  };

  enum class Remote_Client_State
  {
    Version,
    Name,
    Inventory,
    Team_Id,
    Playing
  };

  struct Remote_Client
  {
    ENetPeer* peer;

    Remote_Client_State state;

    Version_Info version;
    std::string player_name;
    Inventory inventory;
    team_id team;

    // Somehow limit this to a certain amount? Circular buffer?
    std::vector<Input> inputs;
  };

  struct Server_Context
  {
    // Most up to date server info
    // This structure includes rules, teams and players.
    // Hmm, those indices may not be correct for us, may need to be adjusted
    // for each client? Also I like this arbitrary structure, maybe it would be
    // better to use templates + macros or something so that each structure
    // could be fairly flat.

    Server_Rules rules;
    std::vector<Team> teams;

    // Player id from the Player_Info struct are for this map
    ID_Map<Remote_Client> clients;

    uint16_t port;
    // This is a little wasteful but whatever.
    uint16_t max_peers;
    Host host;

    bool must_resend_server_info = false;
  };

  void init_server(Server_Context& ctx) noexcept;

  void step_server(Server_Context& context, ENetEvent const& event) noexcept;

} }
