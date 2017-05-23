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
#include "lua_init_engine_bytecode.h"
#include "lua_redc_ffi_bytecode.h"
#include "lua_run_engine_bytecode.h"

#pragma GCC diagnostic pop
#pragma clang diagnostic pop

#define RET_ON_ERR(err) if(err) return err

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

  int load_c_functions(lua_State* L)
  {
    int err = 0;

    // Load our ffi loading code
    err = luaL_loadbuffer(L, luaJIT_BC_redc_ffi, luaJIT_BC_redc_ffi_SIZE,
                          "redc_ffi");
    RET_ON_ERR(err);

    // Run it
    err = lua_pcall(L, 0, 0, 0);

    return err;
  }

  // We can't really separate config loading and engine lua init because the
  // config table is tightly coupled to both.
  int load_engine_lua(lua_State* L, boost::filesystem::path const& cfg_path)
  {
    int err = 0;

    // Load our init code
    err = luaL_loadbuffer(L, luaJIT_BC_init_engine,
                          luaJIT_BC_init_engine_SIZE, "init_engine");
    RET_ON_ERR(err);

    // Load the mod config script
    err = luaL_loadfile(L, cfg_path.string().c_str());
    RET_ON_ERR(err);

    // Use a sandbox (without redcrane / engine functionality for the time
    // being) to run the mod config
    set_sandbox_env(L, -1, false);

    // Run the mod config script, we expect a table back.
    err = lua_pcall(L, 0, 1, 0);
    RET_ON_ERR(err);

    // Use the table on a stack as a parameter to our internal init code.
    err = lua_pcall(L, 1, 0, 0);

    return err;
  }

  int run_engine(lua_State* L, const char* server_mode)
  {
    // Push our engine game loop
    luaL_loadbuffer(L, luaJIT_BC_run_engine, luaJIT_BC_run_engine_SIZE,
                    "run_engine");

    // Push the server mode
    lua_pushstring(L, server_mode);
    // Make a sandbox
    push_sandbox_env(L, true);

    // Call the game loop
    if(handle_err(L, lua_pcall(L, 2, 1, 0)))
    {
      // Error, return a bad error code
      return EXIT_FAILURE;
    }

    // Now double check our return value
    if(!lua_isnumber(L, -1))
    {
      // We are either getting a string that is a valid integer, or something
      // completely ridiculous. This is not an valid issue to crash, but still
      // is incorrect.
      log_w("Mod must return an integer return code (type found: %)",
            lua_type(L, -1));
    }

    // Now get the return value as an integer and return that
    int ret = lua_tointeger(L, -1);

    // Pop the return code and the engine
    lua_pop(L, 2);

    return ret;
  }

  void push_sandbox_env(lua_State* L, bool include_redcrane)
  {
    // If necessary, cache the function to generate each respective sandbox
    // in the registry and then retrieve them as necessary. Right now this
    // ought to run fine.

    // Load the sandbox code, it will return a function that generates the
    // new sandbox on the fly.
    luaL_loadbuffer(L, luaJIT_BC_sandbox, luaJIT_BC_sandbox_SIZE, "sandbox");

    // Tell it whether we want the sandbox or not
    lua_pushboolean(L, include_redcrane);

    // Run it, if it fails though bail out immediately
    if(handle_err(L, lua_pcall(L, 1, 1, 0)))
    {
      // If this failed we shouldn't continue
      redc::crash();
    }

    // Call the function to build the sandbox
    if(handle_err(L, lua_pcall(L, 0, 1, 0)))
    {
      redc::crash();
    }

    // We are left with a new sandbox at the top of the stack
  }
  void set_sandbox_env(lua_State* L, int fn, bool include_redcrane)
  {
    push_sandbox_env(L, include_redcrane);

    // Use the returned table as the sandbox for a given function
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
