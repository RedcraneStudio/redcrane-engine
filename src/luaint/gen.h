/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#pragma once

#include <string>
#include <vector>
#include "../gen/terrain.h"

#include "common.h"

extern "C"
{
  #include "lua.h"
}
namespace game { namespace luaint
{
  struct Terrain_Gen_Config
  {
    void add_option(std::string const& name, double def) noexcept;
  private:
    enum class Option_Type
    {
      Double
    };
    struct Option
    {
      std::string name;
      Option_Type type;
      union
      {
        double d_num;
      } value;
    };
    std::vector<Option> options_;
  };

  struct Terrain_Gen_Mod
  {
    // Index into registry[naeme] where naeme is some implementation (?)
    // defined string.
    // The value of the registry[naeme][table_index] is the mod lua function.
    size_t table_index;
    Terrain_Gen_Config config;
  };
  using Terrain_Gen_Mod_Vector = std::vector<Terrain_Gen_Mod>;

  // Use this to initialize a table to return from a lua require call.
  Table terrain_preload_table(Terrain_Gen_Mod_Vector&) noexcept;

  // Use this to push a grid
  void push_grid_table(lua_State*, gen::Grid_Map&) noexcept;

  void run_mods(lua_State*, Terrain_Gen_Mod_Vector const&, gen::Grid_Map&);
} }
