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

  bool is_click(Mouse_State const& ns, Mouse_State const& os) noexcept;
  bool is_hover(Mouse_State const& ns, Mouse_State const& os) noexcept;
  bool is_drag(Mouse_State const& ns, Mouse_State const& os) noexcept;
} }
