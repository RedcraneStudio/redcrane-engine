/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../map/structure.h"
namespace game { namespace strat
{
  enum class Player_State_Type
  {
    Nothing,
    Building,
    Pie_Menu,
  };

  struct Nothing_State { };

  struct Pie_Menu_State { };

  struct Building_State
  {
    Structure const* to_build;
  };

  struct Player_State
  {
    Player_State_Type type;
    union
    {
      Nothing_State nothing;
      Building_State building;
      Pie_Menu_State pie_menu;
    };
  };
} }
