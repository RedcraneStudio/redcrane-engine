/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "protocol.h"
#include "../common/crash.h"
namespace redc { namespace net
{
  Client make_client(std::string addr, uint16_t port) noexcept
  {
    Client client;
    client.host = std::move(*make_client_host().ok());

    connect_with_client(client.host, addr, port);

    return client;
  }
  Client wait_for_connection(std::string addr, uint16_t port) noexcept
  {
    auto client = make_client(addr, port);
    client.peer = wait_for_connection(client.host, 1000);
    // Failed to connect if peer is null
    if(!client.peer)
    {
      crash("Failed to connect");
    }
    return client;
  }

  void close_connection(Client& client) noexcept
  {
    enet_peer_disconnect(client.peer, 0);
    enet_host_service(client.host.host, NULL, 500);
    enet_host_destroy(client.host.host);
  }

  void wait_for_game_info(Client& client, sail::Game& game) noexcept
  {
  }
  void set_name(Client& client, std::string name) noexcept
  {
  }
} }
