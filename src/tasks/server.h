/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../common/task.h"
#include "../sail/game_struct.h"
#include "../io/net_io.h"
#include "../rpc/msgpack_interface.h"
#include "../rpc/dispatch.h"
namespace redc
{
  struct Network_Client
  {
    // This is a little bit redundant but retains type information.
    Maybe_Owned<Net_IO> io;
    rpc::Msgpack_Interface plugin;
  };

  //! \brief Context information for methods called by a given client.
  struct Method_Info
  {
    Network_Client* client;
    sail::Game* game;
  };

  struct Server_Task : public Task
  {
    Server_Task(sail::Game& game, uint16_t port, uint16_t max_peers) noexcept;
    void step() noexcept override;
  private:
    sail::Game* game_;
    net::Host host_;
    ID_Map<Network_Client> clients_;

    std::vector<rpc::method_t> methods_;
  };

  using client_id = ID_Map<Network_Client>::id_type;

  Network_Client make_network_client(Maybe_Owned<Net_IO> io) noexcept;

  /*
   * \brief Dispatches connections to a host in the form of network clients.
   */
  client_id try_client_dispatch(net::Host& host, ENetEvent const& event,
                                ID_Map<Network_Client>& clients) noexcept;
  client_id check_client_disconnect(ENetEvent const& event,
                                    ID_Map<Network_Client>& clients) noexcept;
}
