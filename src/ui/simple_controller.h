/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "element.h"
#include "mouse_logic.h"
#include "../common/vec.h"
namespace redc { namespace ui
{
  struct Simple_Controller
  {
    bool step(Shared_Element root, Mouse_State ns) noexcept;
  private:
    Mouse_State old_mouse_;
  };
} }
