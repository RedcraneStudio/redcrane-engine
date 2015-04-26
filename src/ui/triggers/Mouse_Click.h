/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../Event_Trigger.h"
#include "../View.h"
namespace game { namespace ui
{
  struct Mouse_Click : public Event_Trigger
  {
    template <class F1, class F2>
    explicit Mouse_Click(F1 const& c, F2 const& uc,
                         bool use_parent_vol = false) noexcept
                         : on_click_(c), on_unclick_(uc),
                           use_parent_vol_(use_parent_vol) {}

    bool try_trigger(View& v,
                       SDL_Event const& event) const noexcept override;
  private:
    std::function<void (Vec<int>)> on_click_;
    std::function<void (Vec<int>)> on_unclick_;
    bool use_parent_vol_;
  };
} }
