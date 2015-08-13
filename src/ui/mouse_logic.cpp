/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "mouse_logic.h"
namespace game { namespace ui
{
  void On_Click_Handler::operator()(Mouse_State const& ms) noexcept
  {
    if(ms.buttons & button_)
    {
      cb_(ms);
    }
  }
  void On_Release_Handler::operator()(Mouse_State const& ms) noexcept
  {
    if(clicked_ && !(ms.buttons & button_))
    {
      cb_(ms);
      clicked_ = false;
    }
    else if(ms.buttons & button_)
    {
      clicked_ = true;
    }
  }
} }
