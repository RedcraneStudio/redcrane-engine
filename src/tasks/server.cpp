/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "server.h"
#include "../common/log.h"
#include "../sail/methods.h"
namespace redc
{
  Server_Task::Server_Task(sail::Game& game, uint16_t port,
                           uint16_t max_peers) noexcept
                           : game_(&game)
  {
    auto host_result = net::make_server_host(port, max_peers);
    if(host_result.err())
    {
      log_e("Failed to start server");
    }

    host_ = std::move(*host_result.ok());

    methods_ = sail::make_server_methods();
  }

  boost::optional<rpc::Request>
    dispatch_request(rpc::Request req,std::vector<rpc::method_t> const& methods,
                     void* userdata) noexcept
  {
    rpc::Run_Context ctx;

    if(req.id) ctx.should_make_params = true;
    else ctx.should_make_params = false;

    ctx.userdata = userdata;

    // Dispatch those requests
    rpc::dispatch(methods, req, &ctx);

    // If we don't need a response or we don't have one, move on
    if(!req.id || !ctx.response) return boost::none;
    else if(ctx.response->object.get().type == msgpack::type::ARRAY)
    {
      // If the request has an id and the function returned some parameters
      // we should send them back.

      // Send back a response if necessary.

      rpc::Request response;
      response.id = req.id;
      response.params = std::move(*ctx.response);
      if(ctx.is_error)
      {
        response.fn = 1;
      }
      else
      {
        response.fn = 0;
      }

      return response;
    }
    return boost::none;
  }

  void Server_Task::step() noexcept
  {
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

        auto response = dispatch_request(req, methods_, &info);
        if(response)
        {
          net_client.plugin.post_request(*response);
        }
      }
    }
  }
  Network_Client make_network_client(Maybe_Owned<Net_IO> io) noexcept
  {
    rpc::Msgpack_Interface msgpack(std::move(io));
    return Network_Client{std::move(io), std::move(msgpack)};
  }
  client_id try_client_dispatch(net::Host& host, ENetEvent const& event,
                                ID_Map<Network_Client>& clients) noexcept
  {
    if(event.type == ENET_EVENT_TYPE_CONNECT)
    {
      // It's stupid to take the whole host in as a constructor parameter but
      // only use it here, is there a better solution?
      auto io = make_maybe_owned<Net_IO>(&host, event.peer);
      auto id = clients.emplace(std::move(io),
                                rpc::Msgpack_Interface{ref_mo(io)});
      return id;
    }
    // Not our event, signal it with a zero, which is never a valid id.
    return 0;
  }

  client_id check_client_disconnect(ENetEvent const& event,
                                    ID_Map<Network_Client>& clients) noexcept
  {
    if(event.type == ENET_EVENT_TYPE_DISCONNECT)
    {
      auto client_id = 0;
      for(auto& client : clients)
      {
        // Is it any of these peers?
        if(client.second.io->peer() == event.peer)
        {
          // Our client disconnected.
          client_id = client.first;
          break;
        }
      }
      if(client_id)
      {
        // We are going to let Server_Task do this part.
        // Remove all players that this client owned.
        //auto ids = s.players.ids();
        //for(auto player_id : ids)
        //{
          //if(s.players.at(player_id).owner == client_ptr)
          //{
            //s.players.erase(player_id);
          //}
        //}

        // Don't forget to reserve its id.
        clients.erase(client_id);
        return client_id;
      }

    }
    // Not our event :/
    return 0;
  }
}
