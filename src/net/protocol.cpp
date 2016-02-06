/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "protocol.h"
#include "../common/crash.h"
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

#define CLIENT_PROTOCOL_VERSION 1

#define REQUIRE_EVENT(ctx, event, type_name) \
  if(!event) break; \
  if(event->type != ENET_EVENT_TYPE_##type_name) break

#define REQUIRE_EVENT_FROM_SERVER(ctx, event) \
  if(ctx.server_peer != event->peer) break

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

          // Protocol version is internal
          version_t cur_prot_version = CLIENT_PROTOCOL_VERSION;

          // Send both
          Version_Info version{cur_prot_version, ctx.client_version};
          send_data(version, ctx.server_peer);

          // Now we are waiting for a response (or failure due to versioning)
          set_state(res, ctx, Client_State::Waiting_For_Info);

          // We don't have any packet to destroy
          set_event_handled(res);
        }

        break;
      }
      case Client_State::Waiting_For_Info:
      {
        REQUIRE_EVENT(ctx, event, RECEIVE);
        REQUIRE_EVENT_FROM_SERVER(ctx, event);

        Version_Okay version_okay;

        // Try to decode Server_Info struct
        if(recieve_data(ctx.server_info, event->packet))
        {
          // The client code should now populate the inventory field.
          set_state(res, ctx, Client_State::Sending_Loadouts);
          set_event_handled(res, event->packet);
        }
        else if(recieve_data(version_okay, event->packet))
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

          if(version_okay.protocol && version_okay.client)
          {
            log_e("Versions seem fine, but something failed!");
          }

          // Either case disconnect
          log_e("Disconnecting...");
          enet_peer_disconnect(event->peer, 0);

          // We did handle this event
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

  void init_server(Server_Context& ctx) noexcept
  {
    ctx.host = std::move(*make_server_host(ctx.port, ctx.max_peers).ok());
  }

  void step_remote_client(Server_Context& ctx,
                          Client_State_And_Buffer& client,
                          ENetEvent const& event) noexcept
  {
    // We can assume we got a recieve event I guess.
    switch(client.state)
    {
      case Remote_Client_State::Version:
      {
        // Recieve version
        if(!recieve_data(client.version, event.packet))
        {
          // Err, ignore:
          break;
        }

        // Make sure the version is correct
        Version_Okay versions_okay;
        versions_okay.protocol =
          client.version.protocol_version == CLIENT_PROTOCOL_VERSION;
        versions_okay.client = client.version.client_version == 1;

        if(!versions_okay.protocol || !versions_okay.client)
        {
          // Send which version was actually the issue.
          send_data(versions_okay, event.peer);
          if(!versions_okay.protocol && versions_okay.client)
          {
            log_i("Rejecting client (game version = %) because we don't know "
                  "protocol version %",
                  client.version.client_version,
                  client.version.protocol_version);
          }
          else if(versions_okay.protocol && !versions_okay.client)
          {
            log_i("Rejecting client (protocol version = %) because we don't "
                  "know game client version %",
                  client.version.protocol_version,
                  client.version.client_version);
          }
          else if(!versions_okay.protocol && !versions_okay.client)
          {
            log_i("Rejecting client because we don't know protocol version % "
                  "or game client version %",
                  client.version.protocol_version,
                  client.version.client_version);
          }

          // Disconnect the peer and jump ship.
          enet_peer_disconnect_later(event.peer, 0);

          break;
        }

        // Versions are okay, send our rules / info, etc.

        // TODO: Figure out how to send the info at any point even while playing.
        send_data(ctx.info, event.peer);
        client.state = Remote_Client_State::Inventory;

        break;
      }
      case Remote_Client_State::Inventory:
      {
        if(!recieve_data(client.inventory, event.packet))
        {
          // Failed to read inventory.
          break;
        }
        send_data(true, event.peer);
        break;
      }
      case Remote_Client_State::Team_Id:
      {
        if(!recieve_data(client.team, event.packet))
        {
          // Failed to read team id.
          break;
        }

        Spawn_Information spawn_info;

        // TODO: Fill in spawn information

        send_data(spawn_info, event.peer);
        break;
      }
      case Remote_Client_State::Playing:
      default:
      {
        Input cur_input;
        if(!recieve_data(cur_input, event.packet))
        {
          // Possibly handle other types of packets.
          break;
        }

        client.inputs.push_back(cur_input);

        break;
      }
    }
  }

  Client_Vector_Iter find_client_by_peer(Client_Vector& cts,
                                         ENetPeer* peer) noexcept
  {
    return std::find_if(std::begin(cts), std::end(cts),
      [&peer](auto& client_state)
      {
        return client_state.peer == peer;
      });
  }

  void step_server(Server_Context& ctx, ENetEvent const& event) noexcept
  {
    switch(event.type)
    {
      case ENET_EVENT_TYPE_CONNECT:
      {
        // Oh boy!
        Client_State_And_Buffer client;

        // We only know this information, we'll fill out the rest later.
        client.peer = event.peer;
        client.state = Remote_Client_State::Version;

        // Keep track of it, of course
        ctx.clients.push_back(std::move(client));

        // At this point we will recieve it's version info.
        break;
      }
      case ENET_EVENT_TYPE_RECEIVE:
      {
        // Progress the state of the client. Jesus Christ that scares me.
        // Abstract that, obviously!

        auto client_find = find_client_by_peer(ctx.clients, event.peer);

        // Find the corect client state and buffer object that will tell us
        // what we are actually recieving
        if(client_find == ctx.clients.end())
        {
          // I don't know when this would happen. Something went quite wrong
          log_e("Recieving data from unknown client - ignoring!");
          break;
        }

        // Recieve whatever we need to recieve, etc.
        step_remote_client(ctx, *client_find, event);

        // Don't forget to destory the packet!
        enet_packet_destroy(event.packet);

        break;
      }
      case ENET_EVENT_TYPE_DISCONNECT:
      default:
      {
        log_i("Disconnect event");

        // TODO:
        // Find an iterator to the right client and remove it from the list of
        // clients.
        auto client_find = find_client_by_peer(ctx.clients, event.peer);

        if(client_find == ctx.clients.end())
        {
          // wat
        }
        else
        {
          // Remove it from our clients list.
          ctx.clients.erase(client_find);
          // TODO: Worry about removing it from teams, etc.
        }

        break;
      }
    }
  }
} }
