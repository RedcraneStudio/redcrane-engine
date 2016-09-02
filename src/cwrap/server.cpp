/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file provides the implementation for the engine's C interface,
 * specifically tailored for LuaJIT's FFI facilities.
 *
 * Server stuff
 */

#include "redcrane.hpp"

using namespace redc;

extern "C"
{
  void redc_server_req_player(void *eng)
  {
    auto rce = (Engine*) eng;
    REDC_ASSERT_HAS_SERVER(rce);

    rce->server->req_player();
  }

  int redc_server_poll_event(void *eng, Redc_Event *event)
  {
    auto rce = (Engine*) eng;
    REDC_ASSERT_HAS_SERVER(rce);

    Server_Event server_event;
    if(rce->server->poll_physics_event(server_event))
    {
      if(server_event.decl)
      {
        event->name = server_event.decl->event_name.c_str();
        return true;
      }
    }
    return false;
  }
}
