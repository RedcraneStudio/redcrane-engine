/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 *
 * \file mouse_logic.h This file provides a few helper functions for
 * controllers.
 */
#pragma once
#include "../common/vec.h"
namespace game { namespace ui
{
  struct Mouse_State
  {
    bool button_down;
    Vec<int> position;
  };

  // Bool - True: Ignore mouse movement, False: Only consider it a click if
  // position before and after is the same.
  bool is_click(Mouse_State const& ns, Mouse_State const& os,
                bool = false) noexcept;
  bool is_release(Mouse_State const& ns, Mouse_State const& os,
                  bool = false) noexcept;
  bool is_hover(Mouse_State const& ns, Mouse_State const& os) noexcept;
  bool is_drag(Mouse_State const& ns, Mouse_State const& os) noexcept;
} }
