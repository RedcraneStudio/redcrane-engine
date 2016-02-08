/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <initializer_list>
#include <vector>
#include "maybe_owned.hpp"
namespace redc
{
  struct Task
  {
    virtual ~Task() noexcept {}
    virtual void step(float dt) noexcept {};
  };

  struct Task_Composite : public Task
  {
    using task_t = Maybe_Owned<Task>;

    Task_Composite(std::initializer_list<task_t> init) noexcept;

    void step(float dt) noexcept override;
  private:
    std::vector<task_t> tasks_;
  };
}
