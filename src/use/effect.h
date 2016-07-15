/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../gfx/idriver.h"
#include "../gfx/camera.h"
namespace redc
{
  namespace gfx
  {
    struct Effect
    {
      virtual ~Effect() noexcept {}

      virtual void init(IDriver& driver) noexcept = 0;
      virtual void render(IDriver& driver, gfx::Camera const& cam) noexcept = 0;
    };
  }
}
