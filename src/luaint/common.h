/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#pragma once
#include <string>
namespace game { namespace luaint
{
  struct Lua;

  Lua* init_lua() noexcept;
  void uninit_lua(Lua* lua) noexcept;

  void load_mod(Lua& L, std::string mod_name);
} }
