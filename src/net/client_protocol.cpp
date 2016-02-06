/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "client_protocol.h"
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
          Version_Info version{CLIENT_PROTOCOL_VERSION, ctx.client_version};
          send_data(version, ctx.server_peer);

          // Now we are waiting for a response
          set_state(res, ctx, Client_State::Waiting_For_Version_Confirmation);

          // We don't have any packet to destroy
          set_event_handled(res);
        }

        break;
      }
      case Client_State::Waiting_For_Version_Confirmation:
      {
        REQUIRE_EVENT(ctx, event, RECEIVE);
        REQUIRE_EVENT_FROM_SERVER(ctx, event);

        Version_Okay version_okay;
        if(recieve_data(version_okay, event->packet))
        {
          if(version_okay.protocol && version_okay.client)
          {
            set_state(res, ctx, Client_State::Sending_Name);
          }
          else
          {
            // Was our protocol version rejected, client version rejected?
            // Or both?

            if(!version_okay.protocol)
            {
              log_e("Server rejected client protocol version: %",
                    CLIENT_PROTOCOL_VERSION);
            }
            if(!version_okay.client)
            {
              log_e("Server rejected client (game) version: %",
                    ctx.client_version);
            }

            // Either case disconnect
            log_e("Disconnecting...");
            enet_peer_disconnect(event->peer, 0);
          }

          // We did handle this event
          set_event_handled(res, event->packet);
        }
        break;
      }
      case Client_State::Sending_Name:
      {
        send_data(ctx.player_name, event->peer);
        set_state(res, ctx, Client_State::Waiting_For_Info);
        break;
      }
      case Client_State::Waiting_For_Info:
      {
        REQUIRE_EVENT(ctx, event, RECEIVE);
        REQUIRE_EVENT_FROM_SERVER(ctx, event);

        // Try to decode Server_Info struct
        if(recieve_data(ctx.server_info, event->packet))
        {
          // The client code should now populate the inventory field.
          set_state(res, ctx, Client_State::Sending_Loadouts);
          set_event_handled(res, event->packet);
        }
        break;
      }
      case Client_State::Sending_Loadouts:
      {
        // Serialize and send inventory
        send_data(ctx.inventory, ctx.server_peer);
        // We are waiting to see if the server accepts our inventory.
        set_state(res, ctx, Client_State::Waiting_For_Inventory_Confirmation);
        break;
      }
      case Client_State::Waiting_For_Inventory_Confirmation:
      {
        REQUIRE_EVENT(ctx, event, RECEIVE);
        REQUIRE_EVENT_FROM_SERVER(ctx, event);

        // Try to decode a bool
        if(recieve_data(ctx.inventory_okay, event->packet))
        {
          // The client code should now populate the team id field.
          // We know our inventory was accepted.
          set_state(res, ctx, Client_State::Sending_Team);
          set_event_handled(res, event->packet);
        }
      }
      case Client_State::Sending_Team:
      {
        // Send team id
        send_data(ctx.team, ctx.server_peer);
        set_state(res, ctx, Client_State::Waiting_For_Spawn);
        break;
      }
      case Client_State::Waiting_For_Spawn:
      {
        REQUIRE_EVENT(ctx, event, RECEIVE);
        REQUIRE_EVENT_FROM_SERVER(ctx, event);

        // Try to decode spawn information
        if(recieve_data(ctx.spawn_information, event->packet))
        {
          set_state(res, ctx, Client_State::Playing);
          set_event_handled(res, event->packet);
        }
        break;
      }
      case Client_State::Playing:
      default:
        break;
    }

    return res;
  }
} }
