/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "methods.h"
#include "../common/log.h"
#include "../tasks/server.h"
namespace redc { namespace sail
{
  std::vector<rpc::method_t> make_server_methods() noexcept
  {
    std::vector<rpc::method_t> ret;

    ret.push_back(handle_response);
    ret.push_back(handle_error);
    ret.push_back(make_conv_method(make_player_server));
    ret.push_back(make_conv_method(move_player_server));

    return ret;
  }
  std::vector<rpc::method_t> make_client_methods() noexcept
  {
    std::vector<rpc::method_t> ret;

    ret.push_back(handle_response);
    ret.push_back(handle_error);
    ret.push_back(make_conv_method(make_player_client));
    ret.push_back(make_conv_method(move_player_client));

    return ret;
  }

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

  void handle_response(rpc::Run_Context*, rpc::Params) noexcept
  {
    // TODO figure out a way to get the id of the client that called the error.
    log_i("Response received");
  }
  void handle_error(rpc::Run_Context*, rpc::Params) noexcept
  {
    log_i("Error received");
  }

  void make_player_server(rpc::Run_Context* ctx, std::string name,
                          Vec<float> pos) noexcept
  {
    auto info = (Method_Info*) ctx->userdata;

    auto id = info->game->players.insert(Player{
      name, true, {}, {}, info->client
    });

    auto& player = info->game->players.at(id);
    if(player.name.empty())
    {
      // Assign them a name from the many species of crane.
      player.name = names[id % name_count];
    }

    collis::reset_force(player.boat_motion);

    player.boat_motion.displacement.displacement.x = pos.x;
    player.boat_motion.displacement.displacement.z = pos.y;

    player.boat_motion.angular.displacement = glm::vec3(0.0f, 0.0f, 0.0f);

    // Return the id of this player
    if(ctx->should_make_params)
    {
      ctx->response = rpc::make_params(id);
      ctx->is_error = false;
    }

    for(auto& client : info->clients)
    {
      rpc::Request req;
      // Check to see if this particular client is the one that made the
      // request in the first place.
      bool owned = false;
      if(&client.second == info->client)
      {
        owned = true;
      }
      // Make the request
      req = req_method(REDC_FN_ID(move_player), id, player.name, pos, owned);
      // Send it out - one to each client.
      client.second.plugin.post_request(req, true);
    }
  }
  void move_player_server(rpc::Run_Context* ctx, player_id player_id,
                          Key key) noexcept
  {
    auto& info = *(Method_Info*) ctx->userdata;

    auto player_iter = info.server->players.find(player_id);
    if(player_iter == info.server->players.end())
    {
      // Return an error, that player id doesn't exist!
      if(ctx->should_make_params)
      {
        ctx->response =
          rpc::make_params(fmt("Player % does not exist", player_id));
        ctx->is_error = true;
      }
    }
    else
    {
      auto& player = player_iter->second;

      // Does this client even own that player?
      if(player.owner == info.client)
      {
        // Yes!

        Vec<float> force;
        switch(key)
        {
        case Key::W:
          force = {0.0f, +1.0f};
        case Key::A:
          force = {-1.0f, 0.0f};
        case Key::S:
          force = {0.0f, -1.0f};
        case Key::D:
          force = {+1.0f, 0.0f};
        default:
          break;
        }
        collis::apply_force(player.boat_motion, glm::vec3(force.x, 0.0f, force.y));

        for(auto& client : info.server->clients)
        {
          rpc::Request req;
          // Make the request
          auto pos = Vec<float>{player.boat_motion.displacement.displacement.x,
                                player.boat_motion.displacement.displacement.y};
          req = req_method(REDC_FN_ID(move_player), player_id, pos, 0.0f);
          // Send it out - one to each client.
          // It's not really important that we send it reliably.
          client.second.plugin.post_request(req, false);
        }
      }
      else
      {
        if(ctx->should_make_params)
        {
          ctx->response =
            rpc::make_params(fmt("You don't own player %", player_id));
          ctx->is_error = true;
        }
      }
    }
  }
  void make_player_client(rpc::Run_Context* ctx, player_id id, std::string name,
                          Vec<float> pos, bool owned) noexcept
  {
    auto& players = *(Local_Player_Map*) ctx->userdata;
    players.insert({id, {owned, name, true, glm::vec3(pos.x, 0.0f, pos.y),
                    glm::vec3(0.0f, 0.0f, 0.0f)}});
  }
  void move_player_client(rpc::Run_Context* ctx, player_id id,
                          Vec<float> pos, float angle) noexcept
  {
    auto& players = *(Local_Player_Map*) ctx->userdata;
    auto player = players.find(id);
    if(player != players.end())
    {
      player->second.displacement.x = pos.x;
      player->second.displacement.z = pos.y;
    }
  }
} }
