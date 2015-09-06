/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#include "common.h"

#include <algorithm>

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
  using Mods = std::vector<Mod_List>;
  struct Lua
  {
    lua_State* state;
    Mods mods;
  };

#define REDC_LANDMASS_GEN 0;
  Lua* init_lua() noexcept
  {
    auto lua = new Lua{luaL_newstate()};
    if(!lua->state)
    {
      log_e("Failed to initialize Lua");
    }

    // Initialize mod types
    lua->mods.emplace_back("Terrain_Algorithm");
    lua->mods.back().set_parameter(0, "func", LUA_TFUNCTION);
    lua->mods.back().set_parameter(1, "config", LUA_TTABLE);

    return lua;
  }
  void uninit_lua(Lua* lua) noexcept
  {
    if(lua->state) lua_close(lua->state);
    delete lua;
  }
  size_t num_mod_types(Lua& lua) noexcept
  {
    return lua.mods.size();
  }
  size_t registered_mods(Lua& lua) noexcept
  {
    size_t sum = 0;

    auto accum = [&sum](auto const& mod)
    {
      sum += mod.num_registered();
    };
    for(auto const& mod : lua.mods) accum(mod);

    return sum;
  }

  void Mod_List::set_parameter(size_t index, std::string const& name,
                               int type_lua) noexcept
  {
    if(parameters_.size() <= index)
    {
      parameters_.resize(index + 1);
    }
    parameters_[index].name = name;
    parameters_[index].type = type_lua;
  }

  int Mod_List::register_mod_fn(lua_State* L)
  {
    // Get our light user data pointer to our mod list
    Mod_List& mod_list =
      *static_cast<Mod_List*>(lua_touserdata(L, lua_upvalueindex(1)));

    // Check each parameter
    for(auto i = 0; i < mod_list.parameters_.size(); ++i)
    {
      luaL_checktype(L, i+1, mod_list.parameters_[i].type);
    }

    // Load the table in the registry, or create it if it isn't already there.
    lua_getfield(L, LUA_REGISTRYINDEX, mod_list.registry_name_.data());
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
    for(auto i = 0; i < mod_list.parameters_.size(); ++i)
    {
      // Do so in reverse order
      auto index = mod_list.parameters_.size() - 1 - i;
      lua_setfield(L, 1, mod_list.parameters_[index].name.data());
    }

    // Push our unregister function with the mod list and index of this table.
    // Our mod list is also being passed as the first upvalue.
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_pushinteger(L, table_index);
    lua_pushcclosure(L, &Mod_List::unregister_mod_fn, 2);
    lua_setfield(L, -2, "unregister");

    // We are left with our new mod table, which has already been inserted into
    // the mods list.

    mod_list.indices_.push_back(table_index);

    // Return it.
    return 1;
  }
  int Mod_List::unregister_mod_fn(lua_State* L)
  {
    // Upvalues: mod list instance, index
    auto mod_list = (Mod_List*) lua_touserdata(L, lua_upvalueindex(1));
    auto index = (size_t) lua_tointeger(L, lua_upvalueindex(2));

    // There is an invariant here: Each value in the indices_ vector is unique.

    using std::begin; using std::end;
    auto find_index =
      std::find(begin(mod_list->indices_), end(mod_list->indices_), index);

    if(find_index != end(mod_list->indices_))
    {
      mod_list->indices_.erase(find_index);
    }

    return 0;
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
    auto& mods = *static_cast<Mods*>(lua_touserdata(L, lua_upvalueindex(1)));

    for(auto& mod : mods)
    {
      // Use the registry name (without the prefix): Make it lowercase,
      // possibly ensure no bad usage of characters (such as a dot) and then
      // use it as a function name.
      auto name = mod.registry_name();

      for(char& c : name)
      {
        if(c == '.')
        {
          c = '_';
        }
        else
        {
          c = std::tolower(c);
        }
      }

      // Name is a reasonable name to use. Prepend the register word and call
      // it a function!
      name = "register_" + name;

      lua_pushlightuserdata(L, &mod);
      lua_pushcclosure(L, &Mod_List::register_mod_fn, 1);
      lua_setfield(L, -2, name.data());
    }

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

    lua_pushlightuserdata(L, &lua.mods);
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
