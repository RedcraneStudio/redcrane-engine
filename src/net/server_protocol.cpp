/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "server_protocol.h"
#include "../common/log.h"
namespace redc { namespace net
{
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

        // Inform the client
        send_data(versions_okay, event.peer);

        if(!versions_okay.protocol || !versions_okay.client)
        {
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

        client.state = Remote_Client_State::Name;

        break;
      }
      case Remote_Client_State::Name:
      {
        if(!recieve_data(client.player_name, event.packet)) break;

        send_data(ctx.info, event.peer);
        client.state = Remote_Client_State::Inventory;

        break;
      }
      case Remote_Client_State::Inventory:
      {
        // Failed to read inventory
        if(!recieve_data(client.inventory, event.packet)) break;

        send_data(true, event.peer);
        client.state = Remote_Client_State::Team_Id;

        break;
      }
      case Remote_Client_State::Team_Id:
      {
        // Failed to read team id.
        if(!recieve_data(client.team, event.packet)) break;

        Spawn_Information spawn_info;

        // TODO: Fill in spawn information

        send_data(spawn_info, event.peer);
        client.state = Remote_Client_State::Playing;

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
          // No need to remove it from teams but we do have to notify everyone
          // that this player no longer exists.
        }

        break;
      }
    }
  }
} }
