/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "server.h"
namespace redc
{
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
      auto io = make_maybe_owned<Net_IO>(host, event.peer);
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
