/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#pragma once
extern "C"
{
  #include "lua.h"
  #include "lauxlib.h"
}
#include <string>

#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>

namespace game { namespace luaint
{
  inline std::string get_string(lua_State* L, size_t index)
  {
    if(lua_type(L, index) == LUA_TSTRING)
    {
      return lua_tostring(L, index);
    }
    luaL_typerror(L, index, "string");
    return "";
  }

  inline double get_number(lua_State* L, size_t index)
  {
    if(lua_type(L, index) == LUA_TNUMBER)
    {
      return lua_tonumber(L, index);
    }
    luaL_typerror(L, index, "number");
    return 0.0;
  }

  // Don't remove the table from the stack!
  struct Table_Ref
  {
    lua_State* L;
    size_t index;

    inline std::string get_string(std::string const& name)
    {
      lua_getfield(L, index, name.data());
      std::string ret = luaint::get_string(L, -1);
      lua_pop(L, -1);
      return ret;
    }
    inline double get_number(std::string const& name)
    {
      lua_getfield(L, index, name.data());
      double ret = luaint::get_number(L, -1);
      lua_pop(L, -1);
      return ret;
    }
  };

  inline Table_Ref get_table(lua_State* L, size_t index)
  {
    if(lua_type(L, index) == LUA_TTABLE)
    {
      return {L, index};
    }
    luaL_typerror(L, index, "table");
    return {L, (size_t) -1};
  }

  template <class T>
  inline T& get_instance(lua_State* L) noexcept
  {
    return *static_cast<T*>(lua_touserdata(L, lua_upvalueindex(1)));
  }

  template <class Func, class T>
  void set_member(lua_State* L, std::string name, Func f, T& value) noexcept
  {
    lua_pushlightuserdata(L, &value);
    lua_pushcclosure(L, f, 1);
    lua_setfield(L, -2, name.data());
  }

  struct Number
  {
    double num = 0.0;
  };
  struct String
  {
    std::string str = "";
  };
  struct Userdata
  {
    bool light = true;
    void* ptr = nullptr;
  };

  struct Table;
  struct Function;
  using Value = boost::variant<boost::recursive_wrapper<Table>, Number, String,
                               Userdata, boost::recursive_wrapper<Function> >;

  struct Table
  {
    using key_t = Value;
    using value_t = Value;

    std::vector<std::pair<key_t, value_t> > values;
  };

  struct Function
  {
    lua_CFunction func;

    std::vector<Value> upvalues;
  };

  void push_value(lua_State* L, Value const&);

  void load_package_lib(lua_State* L);

  // This function assumes luaopen_package has been called.
  void add_require(lua_State* L, std::string name, Value const& t);

  lua_State* init_lua() noexcept;
  void uninit_lua(lua_State* L) noexcept;

  void load_mod(lua_State& L, std::string mod_dir);
} }
