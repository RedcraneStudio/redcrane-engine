/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#include "minilua.h"
#include "common/log.h"
#include "common/debugging.h"

extern "C"
{
  #include "lualib.h"
}

// Disable narrowing conversion warnings
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnarrowing"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"

#include "lua_sandbox_bytecode.h"

#pragma GCC diagnostic pop
#pragma clang diagnostic pop

namespace redc { namespace lua
{
  Scoped_Lua_Init::Scoped_Lua_Init() : lua(luaL_newstate())
  {
    luaL_openlibs(lua);
  }
  Scoped_Lua_Init::~Scoped_Lua_Init()
  {
    if(lua) lua_close(lua);
  }

  // Non sandboxed
  int preload_engine_lua(lua_State* L)
  {
    int err = 0;

    // We compiled this module into the executable so just require it to get it
    // into preload
    if((err = luaL_loadstring(L, "require(\"redcrane\")"))) return err;

    // Now run that code
    if((err = lua_pcall(L, 0, 0, 0))) return err;
  }

  void set_sandbox_env(lua_State* L, int fn)
  {
    // Load the lua chunk that does this for us
    luaL_loadbuffer(L, luaJIT_BC_sandbox, luaJIT_BC_sandbox_SIZE, "sandbox");

    // Run it, if it fails though bail out immediately
    if(handle_err(L, lua_pcall(L, 0, 1, 0)))
    {
      // If this failed we shouldn't continue
      redc::crash();
    }

    // Use the returned table as the sandbox
    lua_setfenv(L, fn);
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
