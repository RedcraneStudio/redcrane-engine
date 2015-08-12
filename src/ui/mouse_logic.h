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
  enum Mouse_Button
  {
    Mouse_Button_Left = 0x01, Mouse_Button_Middle = 0x02,
    Mouse_Button_Right = 0x04
  };

  struct Mouse_State
  {
    unsigned int buttons = 0x00;
    Vec<int> position;
    double scroll_delta = 0.0f;
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
