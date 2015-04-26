/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../../common/vec.h"
#include "../Event_Trigger.h"
namespace game { namespace ui
{
  struct Mouse_Hover : public Event_Trigger
  {
    template <class F1, class F2>
    explicit Mouse_Hover(F1 const& hover, F2 const& unhover,
                         bool use_parent_vol = false) noexcept
                         : on_hover_(hover), on_unhover_(unhover),
                           use_parent_vol_(use_parent_vol) {}
    bool try_trigger(View& v, SDL_Event const& event) const noexcept override;

  private:
    std::function<void (Vec<int>)> on_hover_;
    std::function<void (Vec<int>)> on_unhover_;
    mutable bool is_hovering_ = false;
    bool use_parent_vol_;
  };
} }
