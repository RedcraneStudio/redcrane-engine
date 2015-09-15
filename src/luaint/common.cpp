/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#include "common.h"

#include "../common/log.h"

#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

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
    lua_pushcfunction(L, func.func);
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
    lua_pop(L, -2);

    // Push the value to return
    push_value(L, val);
    // Push our small wrapper of that value for preload
    lua_pushcclosure(L, open_value, 1);

    // Set a field in preload.
    lua_setfield(L, -2, name.data());

    // Remove preload
    lua_pop(L, -1);

    return;
  err:
    log_w("Cannot add module because luaopen_package hasn't been called!");
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

  }
} }
