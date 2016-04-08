/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#ifndef REDCRANE_ENGINE_MAINLUA_H
#define REDCRANE_ENGINE_MAINLUA_H
#include <boost/filesystem.hpp>
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

  // Load just C functions into ffi.C.*, returns a lua error
  int load_c_functions(lua_State* L);

  // Returns the engine on the top of the stack
  int load_engine_lua(lua_State* L, boost::filesystem::path const& cfg_path);

  // Consumes an engine at the top of stack and returns the programs exit code.
  int run_engine(lua_State* L, const char* server_mode);

  void push_sandbox_env(lua_State* L, bool include_redcrane = false);
  void set_sandbox_env(lua_State* L, int fn, bool include_redcrane = false);

  bool handle_err(lua_State* L, int err);
} }
#endif
