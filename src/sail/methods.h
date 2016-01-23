/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../common/vec.h"
#include "../common/utility.h"
#include "../common/glm_vec_serialize.h"
#include "../rpc/dispatch.h"
#include "game_struct.h"

#define REDC_FN_ID(fn) REDC_METHOD_ ## fn

#define REDC_METHOD_handle_response 0
#define REDC_METHOD_handle_error 1
#define REDC_METHOD_make_player 2
#define REDC_METHOD_move_player 3

namespace redc { namespace sail
{
  std::vector<rpc::method_t> make_server_methods() noexcept;
  std::vector<rpc::method_t> make_client_methods() noexcept;

  // Common methods:
  // These should always be located at indices 0 and 1 respectively.
  void handle_response(rpc::Run_Context* ctx, rpc::Params params) noexcept;
  void handle_error(rpc::Run_Context* ctx, rpc::Params params) noexcept;

  enum class Key : uint16_t
  {
    W, A, S, D
  };

  // Server methods:
  void make_player_server(rpc::Run_Context* ctx, std::string name,
                          Vec<float> pos) noexcept;
  void move_player_server(rpc::Run_Context* ctx, player_id,
                          Key key) noexcept;

  // Client methods:
  void make_player_client(rpc::Run_Context* ctx, player_id id, std::string name,
                          Vec<float> pos, bool owned) noexcept;
  void move_player_client(rpc::Run_Context* ctx, player_id id,
                          Vec<float> pos, float angle) noexcept;

  template <class Fn, class... Args>
  rpc::Request req_method(Fn fn, Args&&... args)
  {
    return {static_cast<rpc::fn_t>(fn), boost::none,
            rpc::make_params(std::forward<Args>(args)...)};
  }
  template <class Id, class... Args>
  rpc::Request req_method_with_id(Id fn, rpc::id_t id, Args&&... args)
  {
    return {static_cast<rpc::fn_t>(fn), id,
            rpc::make_params(std::forward<Args>(args)...)};
  }

  template <class... Args>
  rpc::method_t make_conv_method(void(*fn)(rpc::Run_Context*, Args... args))noexcept
  {
    return [fn](rpc::Run_Context* ctx, rpc::Params params)
    {
      std::tuple<Args...> params_tup;
      params.object.get().convert(params_tup);
      call(fn, params_tup, ctx);
    };
  }
} }

MSGPACK_ADD_ENUM(redc::sail::Key);
