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

#include "../common/log.h"

#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>

namespace redc { namespace luaint
{
  inline std::string get_string(lua_State* L, int index)
  {
    if(lua_type(L, index) == LUA_TSTRING)
    {
      return lua_tostring(L, index);
    }
    luaL_typerror(L, index, "string");
    return "";
  }

  inline double get_number(lua_State* L, int index)
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
    int index;

    inline std::string get_string(std::string const& name)
    {
      lua_getfield(L, index, name.data());
      std::string ret = luaint::get_string(L, -1);
      lua_pop(L, 1);
      return ret;
    }
    inline double get_number(std::string const& name)
    {
      lua_getfield(L, index, name.data());
      double ret = luaint::get_number(L, -1);
      lua_pop(L, 1);
      return ret;
    }
  };

  inline Table_Ref get_table(lua_State* L, int index)
  {
    if(lua_type(L, index) == LUA_TTABLE)
    {
      return {L, index};
    }
    luaL_typerror(L, index, "table");
    return {L, -1};
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

  void set_field(lua_State* L, std::string name, Value const&);

  void push_value(lua_State* L, Value const&);

  void load_package_lib(lua_State* L);

  // This function assumes luaopen_package has been called.
  void add_require(lua_State* L, std::string name, Value const& t);

  lua_State* init_lua() noexcept;
  void uninit_lua(lua_State* L) noexcept;

  void load_mod(lua_State& L, std::string mod_dir);

  // Return true on error, use return value to bail out.s
  bool handle_err(lua_State*, int);

  template <class... Args>
  bool handle_err(lua_State* L, int err, std::string fmt, Args&&... args)
  {
    std::string doing = format_str(fmt, std::forward<Args>(args)...);

    if(err == 0) return false;

    // Make sure not to let this get out of date with non-tmpl. handle_err
    // implementation.
    std::string err_msg{lua_tostring(L, -1)};
    switch(err)
    {
    case LUA_ERRRUN:
      log_e("Lua runtime error while '%': %", doing, err_msg);
      break;
    case LUA_ERRMEM:
      log_e("Lua memory error while '%': %", doing, err_msg);
      break;
    case LUA_ERRERR:
      log_e("Lua error running error handler while '%': %", doing, err_msg);
      break;
    case LUA_ERRSYNTAX:
      log_e("Lua syntax error while '%': %", doing, err_msg);
      break;
    case LUA_ERRFILE:
      log_e("Error loading Lua file while '%': %", doing, err_msg);
      break;
    default: // Future errors
      log_e("Unknown Lua error while '%': %", doing, err_msg);
      break;
    }

    return true;
  }
} }
