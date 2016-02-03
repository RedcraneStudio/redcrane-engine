/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <msgpack.hpp>
#include <string>
#include "../common/glm_vec_serialize.h"
#include "net_io.h"
// Move this file somewhere else, we need it and we don't need anything else
// from saillib.
#include "../sail/game_struct.h"
namespace redc { namespace net
{
  // 1. Client initiates a connection to the server, writes version of the
  // client and protocol that it supports, etc.
  // 2. Server response with:
  //    - Server rules
  //    - Player information
  //    - Team information
  //    - Possible place for extensions, etc.
  // or:
  //    - Bad version, etc: Disconnects
  // 3. Client sends its loadouts to the server
  // 4. The server says the inventory will work or no.
  // 3. Client asks the user which team they would like to join. The server
  // may send updates to the client while the client is picking a team.
  // 4. Client -> Server: This team!
  // 5. Server -> Client: Spawn here, with this orientation, approximately this
  // time, with this *seed*, or no!
  // 6. Client -> Server: Sampled input information with time so that the
  // water can be properly simulated, etc. The simulation will run on both
  // systems. The server should also be sending data to the client of updates
  // from other clients, this state should include inputs from the clients so
  // that our client can rewind and apply inputs given its own state of water.
  // This way, each boat looks proper given each clients (sorta) individual
  // water but they stay sorta in sink

  // These roughly corrospond to the above steps, obviously.
  enum class Client_State
  {
    Starting, // For before any of this
    // Populate the server address member
    Connecting, // Waiting for connection acknowledgement
    Waiting_For_Info, // Waiting for players, teams, etc.
    Sending_Loadouts, // About to send the users inventory / loadouts.
    // Populate the inventory member of Client_Context then call step_client
    Waiting_For_Inventory_Confirmation, // Waiting for server okay.
    Sending_Team, // About to send the team the client wanted to be on
    // Populate the team id member of Client_Context then call step_client
    Waiting_For_Spawn, // Waiting for server okay on team and spawn info
    Playing // Your on your own, client
  };

  // First the version information
  using version_t = uint16_t;

  // Server rules
  struct Server_Rules
  {
    uint8_t tickrate;
    uint8_t max_allowed_loadouts;

    MSGPACK_DEFINE(tickrate, max_allowed_loadouts);
  };

  // Player information
  struct Player_Information
  {
    sail::player_id id;
    std::string name;

    MSGPACK_DEFINE(id, name);
  };

  // Yah I'm going to have to say a maximum of 256 teams, sorry.
  // I know... I know.
  using team_id = uint8_t;

  // Teams should be able to have names themselves. Even if the user can't name
  // them themselves, maybe the name could change if every member of the team
  // has a tag on, etc.
  struct Team
  {
    team_id id;
    std::string name;

    // This gives the client a name of the player and an id for future
    // reference.
    std::vector<sail::player_id> members;

    MSGPACK_DEFINE(name, members);
  };

  struct Client_Init_Packet
  {
    Server_Rules rules;
    std::vector<Player_Information> players;
    std::vector<Team> teams;

    MSGPACK_DEFINE(rules, players, teams);
  };

  // Client to server
  struct Loadout
  {
    // Globally agreed upon list of hulls, sails, etc.
    // I guess it is concievable to have more than 256 hulls and sails
    uint16_t hull;
    uint16_t sail;

    uint8_t rudder;
    // For now we are only going to let the user pick two guns
    std::array<uint8_t, 2> guns;

    MSGPACK_DEFINE(hull, sail, rudder, guns);
  };
  struct Inventory
  {
    // This must be the same amount as server max-loadouts.
    std::vector<Loadout> loadouts;

    MSGPACK_DEFINE(loadouts);
  };

  // Hahahaha fuck it
  using okay_t = bool;

  struct Spawn_Information
  {
    glm::vec3 position;
    glm::quat orientation;
    float time;
    uint16_t seed;

    MSGPACK_DEFINE(position, orientation, time, seed);
  };

  // This is for clients to keep track of the server
  struct Client_Context
  {
    Client_State state = Client_State::Starting;
    // Starting
    version_t client_version; // This is not protocol version
    ENetAddress server_addr;
    // Connecting
    net::Host host;
    // Maybe wrap this for RAII; After connecting
    ENetPeer* server_peer;
    // Waiting for server info
    Client_Init_Packet client_init_packet;
    // Sending inventory
    Inventory inventory;
    // Inventory check
    okay_t inventory_okay;
    // Sending team
    team_id team;
    // Spawn
    Spawn_Information spawn_information;
  };

  /*!
   * \brief Steps a client forward in terms of server communication up to
   * Client_State::Playing.
   *
   * \returns Whether or not the event parameter was utilized. If it's false,
   * the event should be handled elsewhere, if it's true ctx probably changed
   * based on the new state in ctx.
   */
  struct Step_Client_Result
  {
    bool context_changed = false;
    bool event_handled = false;
  };
  Step_Client_Result step_client(Client_Context& ctx, ENetEvent const* event) noexcept;

  // I'm thinking the above stuff (player, team) could be idempotent at the
  // cost of the little extra bandwidth. Instead of worrying about specifically
  // constructed packets that say "This team now has this name" we can just
  // resend the definition, we can let the client figure out the differences.

  // Sent from client to server.
  struct Input
  {
    // Standard movement
    // bool forward;
    // bool left;
    // bool right;
    // bool backward;
    // *** SEE BELOW ***

    // Put this in a single byte? From most significant bit to least:
    // Forward, left, right, backward; remaining bits are the client's current
    // weapon.
    uint8_t input;

    // Hmm, I guess the server can make the call as to whether the user shot
    // too earlier with respect to when this changed, then again we can write
    // our own packer that optionally packs this or nothing otherwise, etc.
    // *** SEE ABOVE ***
    // uint8_t cur_weapon;
    // *** SEE ABOVE ***

    // This is the time of the water simulation for the client. Double
    // precision?
    float time;

    MSGPACK_DEFINE(input, time);
  };

  // weapon should be in the range [0,0xff]
  inline Input pack_input(bool forward, bool left, bool right, bool backward,
                          uint8_t weapon, float time) noexcept
  {
    Input ret;

    ret.input = (forward << 7) | (left << 6) | (right << 5) | (backward << 4) |
                (weapon & 0xff);
    ret.time = time;

    return ret;
  }

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

  struct Network_Player
  {
    // What actual player this corrosponds to. Currently we have to do a linear
    // into the vector of players in client_init_packet.
    sail::player_id id;
    std::vector<State> state_queue;
  };

  struct Client
  {
    Client_Context ctx;
    std::vector<Network_Player> players;
  };

  template <class T>
  void send_data(T const& t, ENetPeer* peer) noexcept
  {
    msgpack::sbuffer buf;
    msgpack::pack(buf, t);

    // Make the version packet
    auto packet = enet_packet_create(buf.data(), buf.size(),
                                     ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
  }

} }
