/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <functional>

#include "SDL.h"
namespace game { namespace ui
{
  struct View;

  struct Event_Trigger
  {
    virtual ~Event_Trigger() noexcept {}

    virtual bool try_trigger(View& v,
                             SDL_Event const& event) const noexcept = 0;
  };
} }
