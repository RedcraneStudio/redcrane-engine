/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "protocol.h"
#include "../common/crash.h"
namespace redc { namespace net
{
  struct Version_Info
  {
    version_t protocol_version;
    version_t client_version;

    MSGPACK_DEFINE(protocol_version, client_version);
  };

  Step_Client_Result step_client(Client_Context& ctx, ENetEvent const* event) noexcept
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
        if(!event) break;
        // If it's the correct type of event
        if(event->type == ENET_EVENT_TYPE_CONNECT)
        {
          // From the correct peer? We can probably assume nobody will use our
          // host that we made but better safe than sorry.
          if(event->peer->address.host == ctx.server_addr.host &&
             event->peer->address.port == ctx.server_addr.port)
          {
            res.event_handled = true;
            res.context_changed = true;

            // Now send our version
            ctx.server_peer = event->peer;

            // Serialize versions

            // Protocol version is internal
            version_t cur_prot_version = 1;

            // Send both
            Version_Info version{cur_prot_version, ctx.client_version};
            send_data(version, ctx.server_peer);

            // Now we are waiting for a response (or failure due to versioning)
            ctx.state = Client_State::Waiting_For_Info;
          }
        }
        break;
      }
      case Client_State::Waiting_For_Info:
      {
        if(!event) break;
        // Abstract over actual thing we are reading, etc. Maybe
        if(event->type == ENET_EVENT_TYPE_RECEIVE)
        {
          if(event->peer == ctx.server_peer)
          {
            // We got some data from the server!
            if(event->packet->dataLength == 1)
            {
              // If it is just false:
              if(event->packet->data[0] == 0xc2)
              {
                // Our version was probably bad
                // Throw an error?
              }
            }

            // Try to decode Client_Init_Packet struct
            msgpack::unpacked unpacked;
            msgpack::unpack(unpacked, (const char*) event->packet->data,
                            event->packet->dataLength);
            try
            {
              ctx.client_init_packet = unpacked.get().as<Client_Init_Packet>();
            }
            catch(...)
            {
              // Figure out what exception I need to catch
              // For now rethrow it
              throw;
            }

            // Now we have the client init packet available, that's all we need
            // then the user should populate the inventory field.
            res.context_changed = true;
            res.event_handled = true;
            ctx.state = Client_State::Sending_Loadouts;
          }
        }
        break;
      }
      case Client_State::Sending_Loadouts:
      {
        // Serialize inventory
        send_data(ctx.inventory, ctx.server_peer);

        // We are waiting to see if the server accepts our inventory.
        res.context_changed = true;
        ctx.state = Client_State::Waiting_For_Inventory_Confirmation;
        break;
      }
      case Client_State::Waiting_For_Inventory_Confirmation:
      {
        if(!event) break;
        // This is copied from Waiting_For_Server_Info.
        if(event->type == ENET_EVENT_TYPE_RECEIVE)
        {
          if(event->peer == ctx.server_peer)
          {
            // Try to decode a bool
            msgpack::unpacked unpacked;
            msgpack::unpack(unpacked, (const char*) event->packet->data,
                            event->packet->dataLength);
            try
            {
              ctx.inventory_okay = unpacked.get().as<bool>();
            }
            catch(...)
            {
              // Figure out what exception I need to catch
              // For now rethrow it
              throw;
            }

            // Now we have the client init packet available, that's all we need
            // then the user should populate the inventory field.
            res.context_changed = true;
            res.event_handled = true;
            ctx.state = Client_State::Sending_Team;
          }
        }
        break;
      }
      case Client_State::Sending_Team:
      {
        res.context_changed = true;
        send_data(ctx.team, ctx.server_peer);
        ctx.state = Client_State::Waiting_For_Spawn;
        break;
      }
      case Client_State::Waiting_For_Spawn:
      {
        if(!event) break;
        // This is copied from Waiting_For_Server_Info.
        if(event->type == ENET_EVENT_TYPE_RECEIVE)
        {
          if(event->peer == ctx.server_peer)
          {
            // Try to decode a bool
            msgpack::unpacked unpacked;
            msgpack::unpack(unpacked, (const char*) event->packet->data,
                            event->packet->dataLength);
            try
            {
              ctx.spawn_information = unpacked.get().as<Spawn_Information>();
            }
            catch(...)
            {
              // Figure out what exception I need to catch
              // For now rethrow it
              throw;
            }

            // Now we have the client init packet available, that's all we need
            // then the user should populate the inventory field.
            res.context_changed = true;
            res.event_handled = true;
            ctx.state = Client_State::Playing;
          }
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
