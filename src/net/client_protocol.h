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
  // 2. Server responds with two booleans. Both true means continue. They
  // corrospond to which version were compatible and which weren't.
  // 3. Client sends to the server the player's name.
  // 4. Server response with:
  //    - Server rules
  //    - All players
  //    - All Teams
  //    - ID of the current client
  //    - Possible place for extensions, etc.
  // 5. Client sends its loadouts to the server
  // 6. The server says the inventory will work or no.
  // 7. Client asks the user which team they would like to join. The server
  // may send updates to the client while the client is picking a team.
  // 8. Client -> Server: This team!
  // 9. Server -> Client: Spawn here, with this orientation, approximately this
  // time, with this *seed*, or no!
  // 10. Client -> Server: Sampled input information with time so that the
  // water can be properly simulated, etc. The simulation will run on both
  // systems. The server should also be sending data to the client of updates
  // from other clients, this state should include inputs from the clients so
  // that our client can rewind and apply inputs given its own state of water.
  // This way, each boat looks proper given each clients (sorta) individual
  // water but they stay sorta in sink

  // These roughly corrospond to the above steps, obviously.
  enum class Client_State
  {
    Starting, // For before any of this
    // Populate the server address member
    // Populate version information
    Connecting, // Waiting for connection acknowledgement
    Waiting_For_Version_Confirmation, // Waiting for the server to confirm ver.
    Sending_Name, // About to send the player name
    // Populate player name
    Waiting_For_Info, // Waiting for players, teams, etc.
    Sending_Loadouts, // About to send the users inventory / loadouts.
    // Populate the inventory member of Client_Context then call step_client
    Waiting_For_Inventory_Confirmation, // Waiting for server okay.
    Sending_Team, // About to send the team the client wanted to be on
    // Populate the team id member of Client_Context then call step_client
    Waiting_For_Spawn, // Waiting for server okay on team and spawn info
    Playing // Your on your own, client
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
    uint8_t max_allowed_loadouts;

    MSGPACK_DEFINE(tickrate, max_allowed_loadouts);
  };

  // Yah I'm going to have to say a maximum of 256 teams, sorry.
  // I know... I know.
  using team_id = uint8_t;

  using player_id = uint16_t;

  // Player information
  struct Player_Info
  {
    player_id id;
    std::string name;

    team_id team;

    MSGPACK_DEFINE(id, name);
  };

  template <class T>
  struct ID_Hash
  {
    std::size_t operator()(T const& t) const noexcept
    {
      return static_cast<std::size_t>(t.id);
    }
  };

  using Player_Set = std::set<Player_Info, ID_Hash<Player_Info> >;

  // Teams should be able to have names themselves. Even if the user can't name
  // them themselves, maybe the name could change if every member of the team
  // has a tag on, etc.
  // The id of a team is just it's index into a contigious array of them.
  struct Team
  {
    std::string name;
    MSGPACK_DEFINE(name);
  };

  // Includes meta-information about the game. Each client recieves this as
  // often as things change. It is also sent as part of the connection
  // protocol.
  struct Server_Info
  {
    Server_Rules rules;
    std::vector<Player_Info> players;
    std::vector<Team> teams;

    player_id our_id;

    MSGPACK_DEFINE(rules, players, teams);
  };

  // Client to server
  struct Loadout
  {
    // Globally agreed upon list of hulls, sails, etc.
    // I guess it is concievable to have more than 256 hulls and sails
    uint16_t hull;
    uint16_t sail;

    uint8_t rudder;
    // For now we are only going to let the user pick two guns
    std::array<uint8_t, 2> guns;

    MSGPACK_DEFINE(hull, sail, rudder, guns);
  };
  struct Inventory
  {
    // This must be the same amount as server max-loadouts.
    std::vector<Loadout> loadouts;

    MSGPACK_DEFINE(loadouts);
  };

  struct Spawn_Information
  {
    glm::vec3 position;
    glm::quat orientation;
    float time;
    uint16_t seed;

    MSGPACK_DEFINE(position, orientation, time, seed);
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
    // Sending name
    std::string player_name;
    // Waiting for server info
    Server_Info server_info;
    // Sending inventory
    Inventory inventory;
    // Inventory check
    okay_t inventory_okay;
    // Sending team
    team_id team;
    // Spawn
    Spawn_Information spawn_information;
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

  // Sent from client to server.
  struct Input
  {
    // Standard movement
    // bool forward;
    // bool left;
    // bool right;
    // bool backward;
    // *** SEE BELOW ***

    // Put this in a single byte? From most significant bit to least:
    // Forward, left, right, backward; remaining bits are the client's current
    // weapon.
    uint8_t input;

    // Hmm, I guess the server can make the call as to whether the user shot
    // too earlier with respect to when this changed, then again we can write
    // our own packer that optionally packs this or nothing otherwise, etc.
    // *** SEE ABOVE ***
    // uint8_t cur_weapon;
    // *** SEE ABOVE ***

    // This is the time of the water simulation for the client. Double
    // precision?
    float time;

    MSGPACK_DEFINE(input, time);
  };

  // weapon should be in the range [0,0xff]
  inline Input pack_input(bool forward, bool left, bool right, bool backward,
                          uint8_t weapon, float time) noexcept
  {
    Input ret;

    ret.input = (forward << 7) | (left << 6) | (right << 5) | (backward << 4) |
                (weapon & 0xff);
    ret.time = time;

    return ret;
  }

  // Helper functions to send and recieve.
  template <class T>
  void send_data(T const& t, ENetPeer* peer) noexcept
  {
    msgpack::sbuffer buf;
    msgpack::pack(buf, t);

    // Make the version packet
    auto packet = enet_packet_create(buf.data(), buf.size(),
                                     ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
  }

  template <class T>
  bool recieve_data(T& t, ENetPacket* packet) noexcept
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
