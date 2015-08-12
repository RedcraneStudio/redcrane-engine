/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "mouse_logic.h"
namespace game { namespace ui
{
  // It seems like with this given logic, a mouse click event is made, then a
  // drag event if the user keeps the mouse down, this may get problematic
  // later but for now probably won't be an issue since anything accepting drag
  // events won't care about click events, and vise versa.

  bool left_button(Mouse_State const& ms) noexcept
  {
    return ms.buttons & Mouse_Button_Left;
  }

  bool is_click(Mouse_State const& ns, Mouse_State const& os, bool s) noexcept
  {
    // If we have the user clicked the mouse button down without moving the
    // cursor position that's a click.
    if(s)
      return left_button(ns) && !left_button(os);
    else
      return left_button(ns) && !left_button(os) && ns.position == os.position;
  }
  bool is_release(Mouse_State const& ns, Mouse_State const& os, bool s)noexcept
  {
    if(s)
      return !left_button(ns) && left_button(os);
    else
      return !left_button(ns) && left_button(os) && ns.position == os.position;
  }
  bool is_hover(Mouse_State const& ns, Mouse_State const& os) noexcept
  {
    // We have a changed position and no mouse button down.
    return !left_button(ns) && !left_button(os) && ns.position != os.position;
  }
  bool is_drag(Mouse_State const& ns, Mouse_State const& os) noexcept
  {
    // Like a hover but the buttons must have been down already.
    return left_button(ns) && left_button(os) && ns.position != os.position;
  }
} }
