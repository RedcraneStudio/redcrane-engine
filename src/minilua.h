/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#ifndef REDCRANE_ENGINE_MAINLUA_H
#define REDCRANE_ENGINE_MAINLUA_H
extern "C"
{
  #include "lua.h"
  #include "lauxlib.h"
}
namespace redc { namespace lua
{
  struct Scoped_Lua_Init
  {
    Scoped_Lua_Init();
    ~Scoped_Lua_Init();

    lua_State* lua;
  };
} }
#endif
