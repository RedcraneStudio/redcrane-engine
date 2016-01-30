/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <msgpack.hpp>
#include <string>
#include "net_io.h"
#include "../sail/game_struct.h"
namespace redc { namespace net
{
  // 1. Client initiates a connection to the server. (Possibly write something
  // so we don't accidentally connect to a completely unrelated enet server?)
  // 2. Server response with:
  //    - Player information
  //    - Team information
  //    - Possible place for extensions, etc.
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

  struct Peer
  {
    //Maybe_Owned<net::Host> host;
    ENetPeer* peer;
  };

  // Send from server to client first.
  using version_t = uint16_t;

  struct Player
  {
    std::string name;
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
