/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <vector>
#include <memory>
#include "../common/vec.h"
namespace game { namespace strat
{
  // TODO: Use boost.units or std::ratio or something to make this type safe.
  using ms_t = int;

  using pos_t = Vec<float>;

  struct Msg
  {
    ms_t ttl; // time to live
  };

  struct Msg_Recv_Impl;

  // Message_Recievers are actually shared, but we want to sort of half keep
  // this hidden for now since it may not stay that way.
  struct Msg_Recv
  {
    Msg dequeue_message() noexcept;

    std::shared_ptr<Msg_Recv_Impl> impl;
  };

  struct Msg_Send_Impl;
  struct Msg_Send
  {
    void enqueue_message(Msg) noexcept;

    std::shared_ptr<Msg_Send_Impl> impl;
  };

  struct Message_Dispatcher
  {
    Msg_Recv make_reciever(pos_t) noexcept;
    Msg_Send make_sender(pos_t) noexcept;

    void step(ms_t dt) noexcept;
  private:
    std::vector<Msg_Recv> recievers_;
    std::vector<Msg_Send> senders_;
  };
} }
