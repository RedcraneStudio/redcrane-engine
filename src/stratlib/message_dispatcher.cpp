/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "message_dispatcher.h"
#include "value_map.h"
#include <deque>
namespace game { namespace strat
{
  struct Msg_Recv_Impl
  {
    pos_t pos;
    std::deque<Msg> in_msgs_;
  };
  Msg Msg_Recv::dequeue_message() noexcept
  {
    Value_Map<int16_t> type;
    type.allocate({5, 5});

    auto msg = impl->in_msgs_.back();
    impl->in_msgs_.pop_back();
    return msg;
  }

  struct Msg_Send_Impl
  {
    pos_t pos;
    std::deque<Msg> out_msgs_;
  };

  void Msg_Send::enqueue_message(Msg msg) noexcept
  {
    impl->out_msgs_.push_front(msg);
  }

  Msg_Recv Message_Dispatcher::make_reciever(pos_t pos) noexcept
  {
    auto mrec = std::make_shared<Msg_Recv_Impl>(Msg_Recv_Impl{pos, {}});

    recievers_.push_back(Msg_Recv{mrec});
    return recievers_.back();
  }
  Msg_Send Message_Dispatcher::make_sender(pos_t pos) noexcept
  {
    auto mrec = std::make_shared<Msg_Send_Impl>(Msg_Send_Impl{pos, {}});

    senders_.push_back(Msg_Send{mrec});
    return senders_.back();
  }
} }
