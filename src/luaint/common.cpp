/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#include "common.h"

#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

extern "C"
{
  #include "lualib.h"
}

#include <uv.h>

namespace game { namespace luaint
{
  struct Push_Value_Visitor : public boost::static_visitor<>
  {
    Push_Value_Visitor(lua_State* L) noexcept : L(L) {}

    void operator()(Table const& table) const;
    void operator()(Number const& number) const;
    void operator()(String const& string) const;
    void operator()(Userdata const& userdata) const;
    void operator()(Function const& func) const;

    lua_State* L;
  };

  void Push_Value_Visitor::operator()(Table const& table) const
  {
    lua_newtable(L);
    for(auto const& pair : table.values)
    {
      boost::apply_visitor(*this, pair.first);
      boost::apply_visitor(*this, pair.second);
      lua_settable(L, -3);
    }
  }
  void Push_Value_Visitor::operator()(Number const& number) const
  {
    lua_pushnumber(L, number.num);
  }
  void Push_Value_Visitor::operator()(String const& string) const
  {
    lua_pushstring(L, string.str.data());
  }
  void Push_Value_Visitor::operator()(Userdata const& userdata) const
  {
    if(userdata.light)
    {
      lua_pushlightuserdata(L, userdata.ptr);
    }
    else
    {
      void** dest_ptr = (void**) lua_newuserdata(L, sizeof(void*));
      *dest_ptr = userdata.ptr;
    }
  }
  void Push_Value_Visitor::operator()(Function const& func) const
  {
    for(auto const& value : func.upvalues)
    {
      boost::apply_visitor(*this, value);
    }
    lua_pushcclosure(L, func.func, func.upvalues.size());
  }

  void set_field(lua_State* L, std::string name, Value const& val)
  {
    push_value(L, val);
    lua_setfield(L, -2, name.data());
  }

  void push_value(lua_State* L, Value const& val)
  {
    boost::apply_visitor(Push_Value_Visitor{L}, val);
  }

  int open_value(lua_State* L)
  {
    // Push the actual value.
    lua_pushvalue(L, lua_upvalueindex(1));
    // Return it.
    return 1;
  }

  // TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
  // TODO Not actually a todo, just make sure you use relative indices,    TODO
  // TODO since these functions won't be called from lua and therefore     TODO
  // TODO won't necessarily have an empty stack to begin with. It's        TODO
  // TODO important not to clobber the stack at all.                       TODO
  // TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO

  void add_require(lua_State* L, std::string name, Value const& val)
  {
    lua_getglobal(L, "package");
    if(lua_isnil(L, -1)) goto err;

    lua_getfield(L, -1, "preload");
    if(lua_isnil(L, -1)) goto err;

    // Remove the package table.
    lua_remove(L, -2);

    // Push the value to return
    push_value(L, val);

    // Push our small wrapper of that value for preload
    lua_pushcclosure(L, &open_value, 1);

    // Set a field in preload.
    lua_setfield(L, -2, name.data());

    // Remove preload
    lua_remove(L, -1);

    return;
  err:
    log_w("Cannot add module because luaopen_package hasn't been called!");
  }

  void load_package_lib(lua_State* L)
  {
    lua_pushcfunction(L, &luaopen_package);
    lua_call(L, 0, 0);
  }

  lua_State* init_lua() noexcept
  {
    auto L = luaL_newstate();
    if(!L)
    {
      log_e("Failed to initialize Lua");
    }
    return L;
  }
  void uninit_lua(lua_State* L) noexcept
  {
    if(L) lua_close(L);
  }

  void load_mod(lua_State& L_ref, std::string mod_dir)
  {
    auto L = &L_ref;

    // Switch into that directory
    if(uv_chdir(mod_dir.data()))
    {
      log_w("Failed to switch into mod directory");
      return;
    }

    // Load the main package file.
    auto err = luaL_loadfile(L, "package.lua");
    if(handle_err(L, err)) return;

    err = lua_pcall(L, 0, 0, 0);
    if(handle_if_err(L, err)) return;
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
