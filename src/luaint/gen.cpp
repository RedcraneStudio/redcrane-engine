/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#include "gen.h"

#include "common.h"

extern "C"
{
  #include "lauxlib.h"
}

#define RC_TERRAIN_MODS_REGISTRY_NAME "Redcrane.Gen.Terrain_Mods"
#define RC_GEN_GRID_METATABLE_NAME "Redcrane.Gen.Grid.Metatable"

namespace redc { namespace luaint
{
  void Terrain_Gen_Config::add_option(std::string const& name,
                                      double def) noexcept
  {
    Option opt;
    opt.name = name;
    opt.type = Option_Type::Double;
    opt.value.d_num = def;
    options_.push_back(opt);
  }

  void push_registry_table(lua_State* L, char const* const name) noexcept
  {
    lua_getfield(L, LUA_REGISTRYINDEX, name);
    if(lua_isnil(L, -1))
    {
      // Remove the nil
      lua_remove(L, -1);
      // Add a new table in it's place
      lua_newtable(L);

      // Add it to the registry
      lua_pushvalue(L, -1);
      lua_setfield(L, LUA_REGISTRYINDEX, name);
    }

    // We are left with the table in registry[name] on the top of the stack.
  }

  int unregister_terrain_algorithm(lua_State* L)
  {
    // Get our bound parameters
    auto* mod_list_ptr = lua_touserdata(L, lua_upvalueindex(1));
    auto& mod_list = *static_cast<Terrain_Gen_Mod_Vector*>(mod_list_ptr);

    auto* mod_ptr = lua_touserdata(L, lua_upvalueindex(2));
    auto& mod_ref = *static_cast<Terrain_Gen_Mod*>(mod_ptr);

    // Remove all the mods with this index, there should only be one.
    using std::begin; using std::end;
    auto new_end = std::remove_if(begin(mod_list), end(mod_list),
    [&mod_ref](auto const& mod)
    {
      return mod.table_index == mod_ref.table_index;
    });

    mod_list.erase(new_end, end(mod_list));

    return 0;
  }

  int register_terrain_algorithm(lua_State* L)
  {
    // Parameters
    // - Function
    // - Configuration object

    // Load the list of mods
    push_registry_table(L, RC_TERRAIN_MODS_REGISTRY_NAME);

    // Our list of mods is at the top of the stack
    auto table_index = lua_objlen(L, -1) + 1;

    // Set the value at that index to the function, the configuration object is
    // used to construct a C++ object.
    lua_pushvalue(L, 1);
    lua_rawseti(L, 3, table_index);

    auto* mod_list_ptr = lua_touserdata(L, lua_upvalueindex(1));
    auto& mod_list = *static_cast<Terrain_Gen_Mod_Vector*>(mod_list_ptr);

    Terrain_Gen_Config config;

    // Traverse the configuration object
    lua_pushnil(L);
    while(lua_next(L, 2) != 0)
    {
      if(lua_type(L, -2) == LUA_TSTRING && lua_type(L, -1) == LUA_TNUMBER)
      {
        auto key = lua_tostring(L, -2);
        auto value = lua_tonumber(L, -1);

        config.add_option(key, value);
      }

      // Pop the value, as the remaining key will then be used to find the next
      // one.
      lua_pop(L, 1);
    }

    mod_list.push_back({table_index, config});

    // Build the meta-table that includes an unregister function, for example

    Table ret;

    Function unregister_func{&unregister_terrain_algorithm,
                             {Userdata{true, &mod_list},
                              Userdata{true, &mod_list.back()}}};

    ret.values.emplace_back(String{"unregister"}, std::move(unregister_func));

    push_value(L, ret);
    return 1;
  }

  Table terrain_preload_table(Terrain_Gen_Mod_Vector& mod_v) noexcept
  {
    Table ret;

    ret.values.emplace_back(String{"register_terrain_algorithm"},
                            Function{&register_terrain_algorithm,
                                     {Userdata{true, &mod_v}}});

    return ret;
  }

  int grid_set_tile(lua_State* L)
  {
    auto& grid = **(gen::Grid_Map**)
      luaL_checkudata(L, 1, RC_GEN_GRID_METATABLE_NAME);

    auto pos = get_table(L, 2);

    auto x = pos.get_number("x");
    auto y = pos.get_number("y");

    char const* const opts[] =
    {
      "land",
      "water",
      NULL
    };

    auto vec = Vec<int>(x,y);

    int ind = luaL_checkoption(L, 3, NULL, opts);
    if(ind == 0)
    {
      grid.at(vec).type = gen::Cell_Type::Land;
    }
    else if(ind == 1)
    {
      grid.at(vec).type = gen::Cell_Type::Water;
    }

    return 0;
  }

  int grid_set_contents(lua_State* L)
  {
    auto& grid = **(gen::Grid_Map**)
      luaL_checkudata(L, 1, RC_GEN_GRID_METATABLE_NAME);

    auto pos = get_table(L, 2);

    auto x = pos.get_number("x");
    auto y = pos.get_number("y");

    auto vec = Vec<int>(x,y);

    if(grid.at(vec).type != gen::Cell_Type::Land)
    {
      return 0;
    }

    char const* const opts[] =
    {
      "none",
      "tree",
      NULL
    };
    int ind = luaL_checkoption(L, 3, NULL, opts);
    if(ind == 0)
    {
      grid.at(vec).contents = gen::Cell_Contents::None;
    }
    else if(ind == 1)
    {
      grid.at(vec).contents = gen::Cell_Contents::Tree;
    }

    return 0;
  }

  int grid_get_tile(lua_State* L)
  {
    auto& grid = **(gen::Grid_Map**)
      luaL_checkudata(L, 1, RC_GEN_GRID_METATABLE_NAME);

    auto pos = get_table(L, 2);

    auto x = pos.get_number("x");
    auto y = pos.get_number("y");

    auto vec = Vec<int>(x,y);

    switch(grid.at(vec).type)
    {
    case gen::Cell_Type::Land:
      lua_pushstring(L, "land");
      break;
    case gen::Cell_Type::Water:
      lua_pushstring(L, "water");
      break;
    default:
      lua_pushstring(L, "unknown");
      break;
    }

    return 1;
  }
  int grid_get_contents(lua_State* L)
  {
    auto& grid = **(gen::Grid_Map**)
      luaL_checkudata(L, 1, RC_GEN_GRID_METATABLE_NAME);

    auto pos = get_table(L, 2);

    auto x = pos.get_number("x");
    auto y = pos.get_number("y");

    auto vec = Vec<int>(x,y);

    auto const& grid_cell = grid.at(vec);
    if(grid_cell.type == gen::Cell_Type::Land)
    {
      switch(grid_cell.contents)
      {
      case gen::Cell_Contents::None:
        lua_pushstring(L, "none");
        break;
      case gen::Cell_Contents::Tree:
        lua_pushstring(L, "tree");
        break;
      default:
        lua_pushstring(L, "unknown");
        break;
      }
    }
    else
    {
      lua_pushstring(L, "none");
    }

    return 1;
  }

  void push_grid(lua_State* L, gen::Grid_Map& grid) noexcept
  {
    // New user data.
    auto ud = (gen::Grid_Map**) lua_newuserdata(L, sizeof(gen::Grid_Map**));
    *ud = &grid;

    // Set up the metatable.
    if(luaL_newmetatable(L, RC_GEN_GRID_METATABLE_NAME))
    {
      // Populate it, because it didn't exist before.

      // New table representing '__index'
      lua_newtable(L);

      // function(grid, pos, str type)
      set_field(L, "set_tile", Function{grid_set_tile, {}});
      // function(grid, pos, str type)
      set_field(L, "set_contents", Function{grid_set_contents, {}});
      // function(grid, pos)
      set_field(L, "get_tile", Function{grid_get_tile, {}});
      // function(grid, pos)
      set_field(L, "get_contents", Function{grid_get_contents, {}});

      lua_setfield(L, -2, "__index");
    }

    lua_setmetatable(L, -2);

    // We are left with the userdata at the top of the stack.
  }

  void run_mods(lua_State* L, Terrain_Gen_Mod_Vector const& ml,
                gen::Grid_Map& map)
  {
    // First push the grid map userdata once, we can repush it on top when it's
    // time to pass it in as a parameter.
    push_grid(L, map);

    // Push the function table in the registry

    lua_getfield(L, LUA_REGISTRYINDEX, RC_TERRAIN_MODS_REGISTRY_NAME);
    for(size_t i = 0; i < ml.size(); i++)
    {
      auto const& mod = ml[i];

      // Push the function
      lua_rawgeti(L, -1, mod.table_index);

      // Push the grid
      lua_pushvalue(L, -3);

      // Push the configuration object
      //push_config(L, mod.config);
      lua_pushstring(L, "Way to fucking go Luke");

      int err = lua_pcall(L, 2, 0, 0);
      if(handle_err(L, err, "calling mod #%", i)) return;

      // That's it
    }

    // Get rid of the boilerpot that we added
    lua_pop(L, 2);
  }
} }
