/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#pragma once
#include <string>
#include <vector>

extern "C"
{
  #include <lua.h>
  #include <lauxlib.h>
  #include <lualib.h>
}

namespace game { namespace luaint
{
  struct Lua;

  Lua* init_lua() noexcept;
  void uninit_lua(Lua* lua) noexcept;

  size_t num_mod_types(Lua& lua) noexcept;
  size_t registered_mods(Lua& lua) noexcept;

  struct Mod_Param
  {
    std::string name;
    int type;
  };

  struct Mod_List
  {
    Mod_List(std::string reg_name) noexcept : registry_name_(reg_name) {}

    void set_parameter(size_t index, std::string const& name,
                       int type_lua) noexcept;

    // Pass this to lua, we expect a pointer of an mod_list instance as a value
    // associated to the function (aka closure).
    static int register_mod_fn(lua_State* lua_state);

    // Upvalues: Mod list instance, index in array to remove
    static int unregister_mod_fn(lua_State* lua_state);

    // This string is used in the name used as index into the registry.
    // It is combined with a prefix currently fixed as a macro in our impl.
    // file.
    std::string registry_name() const noexcept { return registry_name_; }

    size_t num_registered() const noexcept { return indices_.size(); }
  private:
    std::string registry_name_;

    // vector of indices into table at registry_name.
    std::vector<size_t> indices_;

    std::vector<Mod_Param> parameters_;
  };

  void load_mod(Lua& L, std::string mod_name);
} }
