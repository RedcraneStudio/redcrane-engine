/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../controller.h"
namespace game { namespace ui
{
  struct Null_Controller : public Controller
  {
    inline void active_rect(Rect const& r) noexcept override {}
    inline void clear_active() noexcept override {}
  };
} }
