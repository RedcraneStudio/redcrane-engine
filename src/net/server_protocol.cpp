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

  Server_Info make_server_info(Server_Context& ctx) noexcept
  {
    // We already have teams and rules.
    // Make a vector of players.
    // A player consists of a name and id.

    auto players = std::vector<Player_Info>{};
    for(auto& client_id_pair : ctx.clients)
    {
      // They will need to select a team at some point.

      // Only if the client has player info do we care about it, otherwise that
      // client probably hasn't finished connecting, etc.
      if(client_id_pair.second.player_info)
      {
        players.push_back(*client_id_pair.second.player_info);
      }
    }

    return Server_Info{ctx.rules, players, ctx.teams};
  }

  bool is_valid_player_id(Server_Context& ctx, player_id id)
  {
    for(auto client_id_pair : ctx.clients)
    {
      // Duplicate!
      if(client_id_pair.second.player_info)
      {
        if(client_id_pair.second.player_info->id == id) return false;
      }
    }
    // No duplicate
    return true;
  }

  void step_remote_client(Server_Context& ctx, Remote_Client& client,
                          player_id client_id, ENetEvent const& event) noexcept
  {
    // We can assume we got a receive event I guess.
    switch(client.state)
    {
      case Remote_Client_State::Version:
      {
        // Receive version
        if(!receive_data(client.version, event.packet))
        {
          // Err, ignore:
          break;
        }

        // Make sure the version is correct
        Version_Okay versions_okay;
        versions_okay.protocol =
          client.version.protocol_version == CLIENT_PROTOCOL_VERSION;
        versions_okay.client = client.version.client_version == 1;

        // How do we stand in terms of compatibility?
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

          // Inform the client
          send_data(versions_okay, event.peer);

          // Disconnect the peer and jump ship.
          enet_peer_disconnect_later(event.peer, 0);
          client.state = Remote_Client_State::Bad_Version;
        }
        else
        {
          // Everything is good, send them the current state of our server!
          send_data(make_server_info(ctx), event.peer);
          client.state = Remote_Client_State::Client_Info;
        }

        break;
      }
      case Remote_Client_State::Client_Info:
      {
        // Read client info, break if it doesn't work
        Player_Info player_info;
        if(!receive_data(player_info, event.packet)) break;

        // Don't add this player info to the client yet so the player id search
        // doesn't find itself and falsely trigger a duplicate

        // Verify user id in some way!
        // For now we are expected random numbers that won't collide, but when
        // it inevitably happens we will disconnect the client for this reason
        if(!is_valid_player_id(ctx, player_info.id))
        {
          // Disconnect the client
          enet_peer_disconnect_later(client.peer, 0);
          log_e("Disconnecting client due to duplicate id");

          client.state = Remote_Client_State::Bad_Id;
        }
        else
        {
          // Else we have a valid player id
          client.player_info = player_info;

          // Otherwise send spawn info
          send_data(Spawn{}, event.peer);
          client.state = Remote_Client_State::Playing;

          // We just added a new player...
          ctx.must_resend_server_info = true;
        }
        break;
      }
      case Remote_Client_State::Playing:
      {
        Input cur_input;
        if(!receive_data(cur_input, event.packet))
        {
          // Possibly handle other types of packets.
        }
        else
        {
          client.inputs.push_back(cur_input);
        }

        break;
      }
    }
  }

  ID_Map<Remote_Client>::iterator
    find_client_by_peer(ID_Map<Remote_Client>& cts, ENetPeer* peer) noexcept
  {
    return std::find_if(std::begin(cts), std::end(cts),
      [&peer](auto& client_state)
      {
        return client_state.second.peer == peer;
      });
  }

  void step_server(Server_Context& ctx, ENetEvent const& event) noexcept
  {
    switch(event.type)
    {
      case ENET_EVENT_TYPE_CONNECT:
      {
        // Oh boy!
        Remote_Client client;

        // We only know this information, we'll fill out the rest later.
        client.peer = event.peer;
        client.state = Remote_Client_State::Version;

        // Keep track of it, of course
        ctx.clients.insert(std::move(client));

        // At this point we will receive its version info.
        break;
      }
      case ENET_EVENT_TYPE_RECEIVE:
      {
        // Progress the state of the client. Jesus Christ that scares me.
        // Abstract that, obviously!

        auto client_find = find_client_by_peer(ctx.clients, event.peer);

        // Find the correct client state and buffer object that will tell us
        // what we are actually receiving
        if(client_find == ctx.clients.end())
        {
          // I don't know when this would happen. Something went quite wrong
          log_e("Receiving data from unknown client - ignoring!");
          break;
        }

        // Receive whatever we need to receive, etc.
        step_remote_client(ctx, client_find->second, client_find->first, event);

        // Don't forget to destroy the packet!
        enet_packet_destroy(event.packet);

        break;
      }
      case ENET_EVENT_TYPE_DISCONNECT:
      {
        log_i("Disconnect event");

        // TODO:
        // Find an iterator to the right client and remove it from the list of
        // clients.
        auto client_find = find_client_by_peer(ctx.clients, event.peer);

        if(client_find == ctx.clients.end())
        {
          // wat
          // RIP
          // A client we don't know just disconnected from us.
          log_w("A client we don't know got disconnected");
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
      case ENET_EVENT_TYPE_NONE:
      default:
        break;
    }

    if(ctx.must_resend_server_info)
    {
      auto server_info = make_server_info(ctx);
      for(auto& client : ctx.clients)
      {
        // First send this identifier which says we are sending this not state.
        send_data(0x01, client.second.peer);

        // Send that info to everyone
        send_data(server_info, client.second.peer);
      }

      // I've found that enet will refuse to clean up a packet if the client
      // we give it has disconnected, which is why we put this after handling
      // events (which importantly includes the disconnect event)
      ctx.must_resend_server_info = false;
    }
  }
} }
