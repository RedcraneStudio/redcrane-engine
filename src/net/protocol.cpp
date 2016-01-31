/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "protocol.h"
#include "../common/crash.h"
namespace redc { namespace net
{
  Step_Client_Result step_client(Client_Context& ctx, ENetEvent const& event) noexcept
  {
    Step_Client_Result res;

    switch(ctx.state)
    {
      case Client_State::Starting:
      {
        // Assuming that the server address has already been set.
        ctx.host = std::move(*make_client_host().ok());
        // How many channels? Just one?
        enet_host_connect(ctx.host.host, &ctx.server_addr, 1, 0);
        ctx.state = Client_State::Connecting;
        res.context_changed = true;
        break;
      }
      case Client_State::Connecting:
      {
        // If it's the correct type of event
        if(event.type == ENET_EVENT_TYPE_CONNECT)
        {
          // From the correct peer? We can probably assume nobody will use our
          // host that we made but better safe than sorry.
          if(event.peer->address.host == ctx.server_addr.host &&
             event.peer->address.port == ctx.server_addr.port)
          {
            res.event_handled = true;
            res.context_changed = true;

            // Now send our version
            ctx.server_peer = event.peer;

            // Serialize version number
            version_t cur_client_version = 1;

            msgpack::sbuffer buf;
            msgpack::pack(buf, cur_client_version);

            // Make the version packet
            auto packet = enet_packet_create(buf.data(), buf.size(),
                                             ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(ctx.server_peer, 0, packet);

            // Now we are waiting for a response (or failure due to versioning)
            ctx.state = Client_State::Waiting_For_Info;
          }
        }
        break;
      }
      case Client_State::Waiting_For_Info:
      case Client_State::Sending_Loadouts:
      case Client_State::Waiting_For_Inventory_Confirmation:
      case Client_State::Sending_Team:
      case Client_State::Waiting_For_Spawn:
        break;
      case Client_State::Playing:
      default:
        break;
    }

    return res;
  }

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
