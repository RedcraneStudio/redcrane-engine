/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../controller.h"
namespace game { namespace ui
{
  struct Input_Controller : public Controller
  {
    void active_rect(Rect const& r) noexcept override;
    void clear_active() noexcept override;

    /*
    void handle_keypress(...)
    void handle_mousemove(...)
    void handle_char(...)
    void handle_scroll(...)
    */
  };
} }
