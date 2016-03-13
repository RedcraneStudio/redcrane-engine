/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <msgpack.hpp>

#include "../common/glm_vec_serialize.h"

#include "net_io.h"
namespace redc { namespace net
{

#define CLIENT_PROTOCOL_VERSION 1
  // 1. Client initiates a connection to the server, sends a packet with:
  //    - Highest client and protocol version that it supports
  // 2. Server may respond in two ways:
  //    a) With two booleans indicated whether it was the client version,
  //       protocol version, or both that made the server reject the
  //       connection.
  //    b) If versions match sufficiently the server should response with:
  //       - Server rules
  //       - All players (Names at this point are sufficient, I guess steam ids
  //         would work.
  //       - All Teams (Clans, whatever)
  //       - List of mods
  // 3. The client may take this opportunity to disconnect if it doesn't
  //    support all the given mods for example. Otherwise it may send the
  //    following information about the player:
  //    - ID
  //    - Name (Important if the ID has never been on that server before.)
  //    - Any other information important for the first visit.
  // 4. If the server finds the given information valid, etc., it may respond
  //    with the clients spawn location.
  // 5. Client -> Server: Sampled input, etc.

  // These roughly correspond to the above steps, obviously.
  enum class Client_State
  {
    Starting, // For before any of this
    // Populate the server address member
    // Populate version information
    Connecting, // Waiting for connection acknowledgement
    Waiting_For_Server_Info, // Waiting for the server info or version fail.
    Sending_Client_Info, // About to send the player name, etc.
    // Populate player name
    Waiting_For_Spawn, // Waiting for spawn location.
    Playing, // Your on your own, client.
    Bad_Version, // The server doesn't support us.
  };

  // Hahahaha fuck it
  using okay_t = bool;

  // First the version information
  using version_t = uint16_t;

  struct Version_Info
  {
    version_t protocol_version;
    version_t client_version;

    MSGPACK_DEFINE(protocol_version, client_version);
  };

  struct Version_Okay
  {
    okay_t protocol;
    okay_t client;

    MSGPACK_DEFINE(protocol, client);
  };

  // Server rules
  struct Server_Rules
  {
    uint8_t tickrate;
    uint8_t max_players;

    MSGPACK_DEFINE(tickrate, max_players);
  };

  // Yah I'm going to have to say a maximum of 65,535 teams, sorry.
  // I know... I know.
  using team_id = uint16_t;

  using player_id = uint16_t;

  // Player information
  struct Player_Info
  {
    player_id id;
    std::string name;

    // Zero for no team association
    team_id team;

    MSGPACK_DEFINE(id, name);
  };

  // Hashs the objects id member
  template <class T>
  struct ID_Hash
  {
    std::size_t operator()(T const& t) const noexcept
    {
      return static_cast<std::size_t>(t.id);
    }
  };

  using Player_Set = std::set<Player_Info, ID_Hash<Player_Info> >;

  // Teams should be able to have names themselves.
  // The id of a team is just it's index into a contiguous array of them.
  struct Team
  {
    std::string name;
    MSGPACK_DEFINE(name);
  };

  // Includes meta-information about the game. Each client receives this as
  // often as things change. It is also sent as part of the connection
  // protocol.
  struct Server_Info
  {
    Server_Rules rules;
    std::vector<Player_Info> players;
    std::vector<Team> teams;

    MSGPACK_DEFINE(rules, players, teams);
  };

  struct Spawn
  {
    // Position of the player
    glm::vec3 position;

    // Angle of the player rotating around the y axis.
    float angle;

    MSGPACK_DEFINE(position, angle);
  };

  // This is for clients to keep track of the server
  struct Client_Context
  {
    Client_State state = Client_State::Starting;
    // Starting
    ENetAddress server_addr;
    // Connecting. Maybe wrap the peer in a RAII class, after connecting
    net::Host host;
    ENetPeer* server_peer;
    // Sending version
    version_t client_version; // This is not protocol version
    // Version confirmation / result
    Version_Okay version_okay;
    // Waiting for server info
    Server_Info server_info;
    // Sending client info
    Player_Info player_info;
    // Spawn
    Spawn spawn;
  };

  /*!
   * \brief Steps a client forward in terms of server communication up to
   * Client_State::Playing.

   * This function will properly destroy a packet it has consumed / used.
   * If event_handled is returned false, the packet should still be usable and
   * must be destroyed by other code.
   *
   * \returns Whether or not the event parameter was utilized. If it's false,
   * the event should be handled elsewhere, if it's true ctx probably changed
   * based on the new state in ctx.
   */
  struct Step_Client_Result
  {
    bool context_changed = false;
    bool event_handled = false;
  };
  Step_Client_Result step_client(Client_Context& ctx,
                                 ENetEvent const* event) noexcept;

  // I'm thinking the above stuff (player, team) could be idempotent at the
  // cost of the little extra bandwidth. Instead of worrying about specifically
  // constructed packets that say "This team now has this name" we can just
  // resend the definition, we can let the client figure out the differences.

  // Helper functions to send and receive.
  template <class T>
  void send_data(T const& t, ENetPeer* peer, bool reliable = true) noexcept
  {
    msgpack::sbuffer buf;
    msgpack::pack(buf, t);

    uint32_t flags = 0;
    if(reliable) flags = ENET_PACKET_FLAG_RELIABLE;


    // Make the version packet
    auto packet = enet_packet_create(buf.data(), buf.size(), flags);
    enet_peer_send(peer, 0, packet);
  }

  template <class T>
  bool receive_data(T& t, ENetPacket* packet) noexcept
  {
    msgpack::unpacked unpacked;
    msgpack::unpack(unpacked, (const char*) packet->data, packet->dataLength);
    try
    {
      // Try to do the conversion
      t = unpacked.get().as<T>();
      // If we get here it worked!
      return true;
    }
    catch(...)
    {
      // We failed to do that conversion
      return false;
    }
  }
} }
