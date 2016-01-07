/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include "../io/net_io.h"
#include "../common/maybe_owned.hpp"
#include "../common/id_map.hpp"
namespace redc { namespace sail
{
  struct Client
  {
    std::string name;
    Maybe_Owned<Net_IO> io;
  };

  struct Server
  {
    ID_Map<Client> clients;
  };

  /*
   * \brief Dispatches connections to a host in the form of clients + net_io.
   */
  struct Client_Dispatcher
  {
    Client_Dispatcher(net::Host& host, Server& server)
      : host_(&host), server_(&server) {}

    // Returns amount of clients added to the server.
    ID_Map<Client>::id_type
      try_client_dispatch(ENetEvent const& event) noexcept;
    ID_Map<Client>::id_type
      check_client_disconnect(ENetEvent const& event) noexcept;

  private:
    net::Host* host_;
    Server* server_;
  };
} }
