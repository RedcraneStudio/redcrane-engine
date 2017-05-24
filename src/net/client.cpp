/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "client.h"
#include "../common/log.h"
namespace redc { namespace net
{
  // These are helper functions so I don't forget to destroy the packet or
  // adjust the right bool flag
  void set_state(Step_Client_Result& res, Client_Context& ctx,
                 Client_State new_state) noexcept
  {
    if(ctx.state == new_state) return;

    ctx.state = new_state;
    res.context_changed = true;
  }

  void set_event_handled(Step_Client_Result& res, ENetPacket* pack = nullptr)
  {
    if(pack)
    {
      enet_packet_destroy(pack);
    }
    res.event_handled = true;
  }

#define REQUIRE_EVENT(ctx, event, type_name) \
  if(!event) break; \
  if(event->type != ENET_EVENT_TYPE_##type_name) break

#define REQUIRE_EVENT_FROM_SERVER(ctx, event) \
  if(ctx.server_peer != event->peer) break

  Step_Client_Result step_client(Client_Context& ctx,
                                 ENetEvent const* event) noexcept
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
        // We are now in the process of connecting! Exciting times!
        set_state(res, ctx, Client_State::Connecting);
        break;
      }
      case Client_State::Connecting:
      {
        REQUIRE_EVENT(ctx, event, CONNECT);

        // From the correct peer? We can probably assume nobody will use our
        // host that we made but better safe than sorry.
        if(event->peer->address.host == ctx.server_addr.host &&
           event->peer->address.port == ctx.server_addr.port)
        {
          // Now send our version
          ctx.server_peer = event->peer;

          // Serialize versions

          // Send both
          Version_Info version{PROTOCOL_VERSION, ctx.client_version};
          send_data(version, ctx.server_peer);

          // Now we are waiting for a response
          set_state(res, ctx, Client_State::Waiting_For_Server_Info);

          // We don't have any packet to destroy
          set_event_handled(res);
        }

        break;
      }
      case Client_State::Waiting_For_Server_Info:
      {
        REQUIRE_EVENT(ctx, event, RECEIVE);
        REQUIRE_EVENT_FROM_SERVER(ctx, event);

        Version_Okay version_okay;
        Server_Info server_info;

        if(receive_data(version_okay, event->packet))
        {
          // Was our protocol version rejected, client version rejected?
          // Or both?

          if(!version_okay.protocol)
          {
            log_e("Server rejected client protocol version: %",
                  PROTOCOL_VERSION);
          }
          if(!version_okay.client)
          {
            log_e("Server rejected client (game) version: %",
                  ctx.client_version);
          }

          // On the very, very off chance we get both of them equal to true
          // it is a logic error on the server side, still disconnect, that
          // server must be broken

          // Either case disconnect
          log_e("Disconnecting due to bad version...");
          enet_peer_disconnect(event->peer, 0);

          // Give the client the actual version response.
          ctx.version_okay = version_okay;
          // And let them know it was the version that failed, stop further
          // processing, etc
          set_state(res, ctx, Client_State::Bad_Version);

          // We did handle this event
          set_event_handled(res, event->packet);
        }
        else if(receive_data(server_info, event->packet))
        {
          // Looks like we got server info on our hands.
          ctx.server_info = server_info;
          set_state(res, ctx, Client_State::Sending_Client_Info);
          set_event_handled(res, event->packet);
        }
        break;
      }
      case Client_State::Sending_Client_Info:
      {
        send_data(ctx.player_info, ctx.server_peer);
        set_state(res, ctx, Client_State::Waiting_For_Spawn);
        break;
      }
      case Client_State::Waiting_For_Spawn:
      {
        REQUIRE_EVENT(ctx, event, RECEIVE);
        REQUIRE_EVENT_FROM_SERVER(ctx, event);

        // Try to decode Spawn information struct
        if(receive_data(ctx.spawn, event->packet))
        {
          // The client code should now populate the inventory field.
          set_state(res, ctx, Client_State::Playing);
          set_event_handled(res, event->packet);
        }
        break;
      }
      case Client_State::Playing:
      {
        if(event)
        {
          if(event->type == ENET_EVENT_TYPE_RECEIVE)
          {
            // Receive game packets
            receive_data(ctx.cur_update, event->packet);
          }
        }
        else
        {
          // No event given

          // Send latest client input with some index so that inputs with the
          // same id can be idempotent.
          send_data(ctx.cur_input, ctx.server_peer);
        }
      }
      case Client_State::Bad_Version:
        break;
    }

    return res;
  }
} }
