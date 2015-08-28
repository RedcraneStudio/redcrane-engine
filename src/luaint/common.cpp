/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#include "common.h"

#include <vector>

#include "../common/log.h"

#include <uv.h>

extern "C"
{
  #include <lua.h>
  #include <lauxlib.h>
  #include <lualib.h>
}

namespace game { namespace luaint
{
  struct Lua
  {
    lua_State* state;
  };

  Lua* init_lua() noexcept
  {
    auto lua = new Lua{luaL_newstate()};
    if(!lua->state)
    {
      log_e("Failed to initialize Lua");
    }
    return lua;
  }
  void uninit_lua(Lua* lua) noexcept
  {
    if(lua->state) lua_close(lua->state);
    delete lua;
  }

  struct Mod_Decl
  {
    size_t table_index;
  };
  using Mod_List = std::vector<Mod_Decl>;

  // No return value
  // Parameters:
  // - Gen function taking two parameters (grid, terrain params)
  // - Configuration table.
  int register_terrain_landmass_func(lua_State* L)
  {
    // Check parameters
    luaL_checktype(L, 1, LUA_TFUNCTION);
    luaL_checktype(L, 2, LUA_TTABLE);

    // Landmass gen mod registry (type: list of tables containing function and
    // the configuration table)
    lua_getfield(L, LUA_REGISTRYINDEX, "Redcrane.Terrain_Landmass_Mods");
    if(lua_isnil(L, -1))
    {
      // Remove nil
      lua_remove(L, -1);

      // Add the registry
      lua_newtable(L);
    }

    // We have our mod registry at the top of the stack

    // Use the next available index in the mod list
    auto table_index = lua_objlen(L, -1) + 1;

    // Insert a table into the mod registry
    lua_newtable(L);

    // Push the table twice so it remains at the top of the stack after being
    // set to the mod list
    lua_pushvalue(L, -1);
    lua_rawseti(L, -3, table_index);

    // Remove the mod list
    lua_remove(L, -2);

    // Move our new table to the bottom of the stack (index 1)
    lua_insert(L, 1);

    // Set the parameters to this table
    lua_setfield(L, 1, "config");
    lua_setfield(L, 1, "func");

    // We are left with our new mod table, which has already been inserted into
    // the terrain landmass mods list.

    // Get our light user data pointer to our mod list
    Mod_List& mod_list =
      *static_cast<Mod_List*>(lua_touserdata(L, lua_upvalueindex(1)));
    mod_list.push_back({table_index});

    // Return the table
    return 1;
  }

  static luaL_Reg redcrane_lua_functions[] =
  {
      {NULL, NULL}
  };

  int luaopen_redcrane(lua_State* L)
  {
    // New table
    lua_newtable(L);

    // Populate it
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_pushcclosure(L, register_terrain_landmass_func, 1);
    lua_setfield(L, -2, "register_terrain_landmass_func");

    luaL_register(L, NULL, redcrane_lua_functions);

    return 1;
  }

  void load_mod(Lua& lua, std::string mod_name)
  {
    lua_State* L = lua.state;
    if(!L)
    {
      log_e("Cannot load mod - bad Lua state");
      return;
    }

    // Load 'require' and friends

    lua_pushcfunction(L, luaopen_package);
    lua_call(L, 0, 0);

    // Add our loader to be potentially 'require'd
    lua_getglobal(L, "package");
    if(lua_isnil(L, 1))
    {
      log_w("Bah, null value!");
    }
    lua_getfield(L, 1, "preload");

    Mod_List mod_list;

    lua_pushlightuserdata(L, &mod_list);
    lua_pushcclosure(L, luaopen_redcrane, 1);

    lua_setfield(L, 2, "redcrane");

    if(uv_chdir(mod_name.data()))
    {
      log_e("Error changing to mod directory");
    }

    auto err = luaL_loadfile(L, "package.lua");
    if(err != 0)
    {
      std::string err_msg{lua_tostring(L, -1)};
      switch(err)
      {
      case LUA_ERRSYNTAX:
        log_e("Lua syntax error: %", err_msg);
        break;
      case LUA_ERRMEM:
        log_e("Lua memory error: %", err_msg);
        break;
      case LUA_ERRFILE:
        log_e("Error loading Lua file: %", err_msg);
        break;
      default: // Future errors
        log_e("Unknown Lua error: %", err_msg);
        break;
      }

      return;
    }

    err = lua_pcall(L, 0, 0, 0);
    if(err != 0)
    {
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
        log_e("Error in Lua error handling: %", err_msg);
        break;
      default: // Future errors
        log_e("Unknown Lua error: %", err_msg);
        break;
      }

      return;
    }
  }
} }
