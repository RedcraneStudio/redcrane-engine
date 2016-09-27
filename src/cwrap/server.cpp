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

    Lua_Event lua_event;
    if(rce->server->poll_lua_event(lua_event))
    {
      if(lua_event.type == Lua_Event::Physics)
      {
        event->type = "physics";
        event->name = lua_event.physics_decl->event_name.c_str();
        event->data = NULL;
        return true;
      }
      if(lua_event.type == Lua_Event::Map_Loaded)
      {
        event->type = "map_loaded";
        event->name = "";
        event->data = (void*) lua_event.map;
        return true;
      }
    }
    return false;
  }
}
