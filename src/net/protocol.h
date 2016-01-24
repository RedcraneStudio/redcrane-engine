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
