/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include "../common/task.h"
#include "../common/vec.h"
#include "SDL.h"
#include "../sail/game_struct.h"
namespace redc
{
  struct Render_Task : public Task
  {
    Render_Task(sail::Game const& game, std::string title, Vec<int> res,
                bool fullscreen, bool vsync) noexcept;
    ~Render_Task() noexcept;
    void step() noexcept override;
  private:
    sail::Game const* game_;
  };
}
