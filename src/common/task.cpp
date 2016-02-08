/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "task.h"
#include <algorithm>
namespace redc
{
  Task_Composite::Task_Composite(std::initializer_list<task_t> init) noexcept
  {
    tasks_.resize(init.size());
    std::move(init.begin(), init.end(), tasks_.begin());
  }
  void Task_Composite::step(float dt) noexcept
  {
    for(auto& task : tasks_)
    {
      task->step(dt);
    }
  }
}

