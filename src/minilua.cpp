/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#include "minilua.h"
#include "common/log.h"

namespace redc { namespace lua
{
  Scoped_Lua_Init::Scoped_Lua_Init() : lua(luaL_newstate()) { }
  Scoped_Lua_Init::~Scoped_Lua_Init()
  {
    if(lua) lua_close(lua);
  }

  bool handle_err(lua_State* L, int err)
  {
    if(err == 0) return false;

    std::string err_msg{lua_tostring(L, -1)};
    switch(err)
    {
      case LUA_ERRRUN:
        log_e("Lua runtime error: %", err_msg);
        break;
      case LUA_ERRMEM:
        log_e("Lua memory error: %", err_msg);
        break;
      case LUA_ERRERR:
        log_e("Lua error running error handler: %", err_msg);
        break;
      case LUA_ERRSYNTAX:
        log_e("Lua syntax error: %", err_msg);
        break;
      case LUA_ERRFILE:
        log_e("Error loading Lua file: %", err_msg);
        break;
      default: // Future errors
        log_e("Unknown Lua error: %", err_msg);
        break;
    }

    return true;
  }
} }
