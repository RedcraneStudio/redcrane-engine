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
  // highest protocol version the client supports.
  // 2. Server response with:
  //    - Player information
  //    - Team information
  //    - Possible place for extensions, etc.
  // or:
  //    - Bad version something, disconnects
  // 3. Client asks the user which team they would like to join. The server
  // may send updates to the client while the client is picking a team.
  // 4. Client -> Server: This team!
  // 5. Server -> Client: Fine, or no!
  // 6. Client has the user pick their boat.
  // 7. Client -> Server: This boat!
  // 8. Server -> Client: Spawn here, which this orientation, approximately
  // this time, with this *seed*, or no!
  // 9. Client -> Server: Sampled input information with time so that the
  // water can be properly simulated, etc. The simulation will run on both
  // systems. The server should also be sending data to the client of updates
  // from other clients, this state should include inputs from the clients so
  // that our client can rewind and apply inputs given its own state of water.
  // This way, each boat looks proper given each clients (sorta) individual
  // water but they stay sorta in sink

  // These roughly corrospond to the above steps, obviously.
  enum class Client_State
  {
    Connecting,
    Waiting_For_Players,
    Selecting_Team,
    Waiting_For_Team,
    Selecting_Boat,
    Waiting_For_Boat,
    Playing
  };

  // First the version information
  using version_t = uint16_t;

  // Player information
  struct Player_Information
  {
    sail::player_id id;
    std::string name;

    MSGPACK_DEFINE(id, name);
  };

  // Teams should be able to have names themselves. Even if the user can't name
  // them themselves, maybe the name could change if every member of the team
  // has a tag on, etc.
  struct Team
  {
    // Yah I'm going to have to say a maximum of 256 teams, sorry.
    // I know... I know.
    uint8_t id;
    std::string name;

    // This gives the client a name of the player and an id for future
    // reference.
    std::vector<Player_Information> members;

    MSGPACK_DEFINE(name, members);
  };

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
    // Forward, left, right, backward; remaining bytes are the client's current
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

  struct Peer
  {
    //Maybe_Owned<net::Host> host;
    ENetPeer* peer;
  };

  // Then the server sends one of these.
  struct Client_Info
  {
    uint8_t map_id;
    std::vector<std::pair<uint16_t, std::string> > players;

    MSGPACK_DEFINE(map_id, players);
  };

  struct Client
  {
    net::Host host;
    ENetPeer* peer;

    //void step() noexcept;
  };

  Client make_client(std::string addr, uint16_t port) noexcept;
  Client wait_for_connection(std::string addr, uint16_t port) noexcept;

  void close_connection(Client& client) noexcept;

  void wait_for_game_info(Client& client, sail::Game& game) noexcept;
  void set_name(Client& client, std::string name) noexcept;

  // General
  void send_data(Peer peer, buf_t const& buf) noexcept;

  // Client
  void send_name(Peer peer, std::string name) noexcept;
  void request_spawn(Peer peer) noexcept;

  void send_protocol_version(Peer peer) noexcept;
  void send_map_id(Peer peer, uint16_t) noexcept;
  void send_player(Peer peer, uint16_t, std::string) noexcept;
  void send_player_id(Peer peer, uint16_t) noexcept;
} }
