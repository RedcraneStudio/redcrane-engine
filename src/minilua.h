/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#ifndef REDCRANE_ENGINE_MAINLUA_H
#define REDCRANE_ENGINE_MAINLUA_H
extern "C"
{
  #include "lua.h"
  #include "lauxlib.h"
}
namespace redc { namespace lua
{
  struct Scoped_Lua_Init
  {
    Scoped_Lua_Init();
    ~Scoped_Lua_Init();

    lua_State* lua;
  };

  // Put in preload important lua engine code that won't run with a sandbox
  int preload_engine_lua(lua_State* L);

  void set_sandbox_env(lua_State* L, int fn);

  bool handle_err(lua_State* L, int err);
} }
#endif
