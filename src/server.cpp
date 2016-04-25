/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#include "server.h"
namespace redc
{
  bool Server_Base::poll_event(Server_Event& event)
  {
    if(event_queue_.size())
    {
      event = event_queue_.front();
      event_queue_.pop();
      return true;
    }
    return false;
  }

  void Server_Base::push_outgoing_event(Server_Event const& event)
  {
    event_queue_.push(event);
  }
}

