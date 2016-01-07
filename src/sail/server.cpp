/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "server.h"
namespace redc { namespace sail
{
  // Right now we allow 12 max clients
  constexpr std::size_t name_count = 12;
  constexpr const char* const names[] = {
    "Grus leucogeranus",
    "Grus canadensis",
    "Grus vipio",
    "Grus antigone",
    "Grus rubicunda",
    "Grus japonensis",
    "Grus americana",
    "Grus grus",
    "Grus monacha",
    "Grus nigricollis",
    "Grus cubensis", // Unfortunately, these Cuban fightless cranes are extinct.
    "Grus pagei" // From California! Extinct though :/
  };

  ID_Map<Client>::id_type
    Client_Dispatcher::try_client_dispatch(ENetEvent const& event) noexcept
  {
    if(event.type == ENET_EVENT_TYPE_CONNECT)
    {
      auto io = make_maybe_owned<Net_IO>(server_->host, event.peer);
      auto id = server_->clients.emplace("", std::move(io));
      // Give it a random name

      // What are we doing? We are assigning it a random name from all the many
      // species of cranes!
      server_->clients.find(id).name = names[(id - 1) % name_count];
      return id;
    }
    // Zero is never a valid id.
    return 0;
  }

  ID_Map<Client>::id_type
    Client_Dispatcher::check_client_disconnect(ENetEvent const& event) noexcept
  {
    if(event.type == ENET_EVENT_TYPE_DISCONNECT)
    {
      auto id = 0;
      for(auto& client : server_->clients)
      {
        if(client.second.io->peer() == event.peer)
        {
          // Our client disconnected.
          id = client.first;
          break;
        }
      }
      if(id)
      {
        server_->clients.erase(id);
        return id;
      }

    }
    // Not our event :/
    return 0;
  }

} }
