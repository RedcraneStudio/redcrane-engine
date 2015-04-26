/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <glm/glm.hpp>
namespace game { namespace ui
{
  struct Rect
  {
    glm::ivec2 pos;
    int width, height;
  };

  struct Controller
  {
    virtual ~Controller() noexcept {}

    virtual void active_rect(Rect const& r) noexcept = 0;
    virtual void clear_active() noexcept = 0;
  };
} }
