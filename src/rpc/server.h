/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../common/task.h"
#include "../common/id_map.hpp"
#include "../common/log.h"
#include "../io/net_io.h"
#include "network_client.h"
#include "msgpack_interface.h"
#include "dispatch.h"
namespace redc
{
  template <class T>
  struct Server_Task : public Task
  {
    Server_Task(T& game, std::vector<rpc::method_t> const& methods,
                uint16_t port, uint16_t max_peers) noexcept;
    void step() noexcept override;

    // Aahh!! Think of a better solution to server methods dispatching other
    // methods to clients. The issue is in a function like make_player that
    // requires a parameter to indicate if this new player is owned by it or
    // not. An awesome functional solution would be to abstract with a functor
    // such that it can either come up with a value based on context or not.
    ID_Map<Network_Client>& clients() noexcept { return clients_; }

    void dispatch_to_all_except(Network_Client& client,
                                rpc::Request const& req,
                                bool rely = true) noexcept;
    void dispatch_to(Network_Client& client,
                     rpc::Request const& req, bool rely = true) noexcept;
    void dispatch_to_all(rpc::Request const& req, bool rely = true) noexcept;
  private:
    T* game_;
    net::Host host_;
    ID_Map<Network_Client> clients_;

    std::vector<rpc::method_t> methods_;
  };

  template <class T>
  Server_Task<T>::Server_Task(T& game,
                              std::vector<rpc::method_t> const& methods,
                              uint16_t port, uint16_t max_peers) noexcept
                              : game_(&game), methods_(methods)
  {
    auto host_result = net::make_server_host(port, max_peers);
    if(host_result.err())
    {
      log_e("Failed to start server");
    }

    host_ = std::move(*host_result.ok());
  }

  template <class T>
  void Server_Task<T>::dispatch_to_all_except(Network_Client& client,
                                              rpc::Request const& req,
                                              bool rely) noexcept
  {
    for(auto& client_pair : clients_)
    {
      if(&client_pair.second != &client)
      {
        client_pair.second.plugin.post_request(req, rely);
      }
    }
  }
  template <class T>
  void Server_Task<T>::dispatch_to(Network_Client& client,
                                   rpc::Request const& req,
                                   bool rely) noexcept
  {
    client.plugin.post_request(req, rely);
  }

  template <class T>
  void Server_Task<T>::dispatch_to_all(rpc::Request const& req, bool rely) noexcept
  {
    for(auto& client_pair : clients_)
    {
      client_pair.second.plugin.post_request(req, rely);
    }
  }

  template <class T>
  void Server_Task<T>::step() noexcept
  {
    // Step each client.
    for(auto& client : clients_)
    {
      // Is this even necessary for Net_IO? I mean we can just use post_recieve.
      client.second.io->step();
    }

    // Handle any net events.
    ENetEvent event;
    while(enet_host_service(host_.host, &event, 0) != 0)
    {
      auto id = try_client_dispatch(host_, event, clients_);
      if(id)
      {
        // That was a connection!
        log_i("Client % connected", id);
      }
      else if((id = check_client_disconnect(event, clients_)))
      {
        log_i("Client % disconnected", id);

        // Remove all the players that that particular client owned.
        std::vector<sail::player_id> ids{};
        for(auto player_pair : game_->players)
        {
          auto owner = (Network_Client*) player_pair.second.userdata;
          if(owner == &clients_.at(id))
          {
            ids.push_back(id);
          }
        }
        for(auto id : ids)
        {
          game_->players.erase(id);
        }
      }
      else
      {
        // Something else?
        for(auto& client_pair : clients_)
        {
          auto& client = client_pair.second;
          if(client.io->post_recieve(event))
          {
            // We just handled the event, because it matched the peer of some
            // client.
            // Break from this loop
            break;
          }
        }
      }
    }

    // For every network client
    for(auto &net_client_pair : clients_)
    {
      auto& net_client = net_client_pair.second;

      // Poll for any new requests.
      rpc::Request req;
      while(net_client.plugin.poll_request(req))
      {
        Method_Info info;
        info.client = &net_client;
        info.game = game_;
        info.server_client = this;

        auto response = dispatch_request(req, methods_, &info);
        if(response)
        {
          net_client.plugin.post_request(*response);
        }
      }
    }
  }

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
