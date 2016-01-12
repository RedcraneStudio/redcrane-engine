/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include "../common/task.h"
#include "../common/vec.h"
#include "SDL.h"
namespace redc
{
  struct Render_Task : public Task
  {
    Render_Task(std::string title, Vec<int> res, bool fullscreen,
                bool vsync) noexcept;
    ~Render_Task() noexcept;
    void step() noexcept override;
  };
}
