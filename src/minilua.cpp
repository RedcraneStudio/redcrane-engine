/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#include "minilua.h"
extern "C"
{
  #include "lualib.h"
}

namespace redc { namespace lua
{
  Scoped_Lua_Init::Scoped_Lua_Init() : lua(luaL_newstate()) { }
  Scoped_Lua_Init::~Scoped_Lua_Init()
  {
    if(lua) lua_close(lua);
  }
} }
