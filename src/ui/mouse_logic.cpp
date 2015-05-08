/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "mouse_logic.h"
namespace game { namespace ui
{
  // It seems like with this given logic, a mouse click event is made, then a
  // drag event if the user keeps the mouse down, this may get problematic
  // later but for now probably won't be an issue since anything accepting drag
  // events won't care about click events, and vise versa.

  bool is_click(Mouse_State const& ns, Mouse_State const& os) noexcept
  {
    // If we have the user clicked the mouse button down without moving the
    // cursor position that's a click.
    return ns.button_down && !os.button_down && ns.position == os.position;
  }
  bool is_hover(Mouse_State const& ns, Mouse_State const& os) noexcept
  {
    // We have a changed position and no mouse button down.
    return !ns.button_down && !os.button_down && ns.position != os.position;
  }
  bool is_drag(Mouse_State const& ns, Mouse_State const& os) noexcept
  {
    // Like a hover but the buttons must have been down already.
    return ns.button_down && os.button_down && ns.position != os.position;
  }
} }
