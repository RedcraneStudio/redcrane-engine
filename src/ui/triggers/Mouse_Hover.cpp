/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "Mouse_Hover.h"
#include "../View.h"
namespace game { namespace ui
{
  bool Mouse_Hover::try_trigger(View& v, SDL_Event const& event) const noexcept
  {
    if(event.type == SDL_MOUSEMOTION)
    {
      auto pt = Vec<int>{event.motion.x, event.motion.y};

      auto volume = Volume<int>{};
      if(use_parent_vol_) volume = v.parent_volume();
      else volume = v.this_volume();

      if(is_in(volume, pt) && !is_hovering_)
      {
        if(on_hover_) on_hover_(pt);
        is_hovering_ = true;
        return true;
      }
      else if(!is_in(volume, pt) && is_hovering_)
      {
        if(on_unhover_) on_unhover_(pt);
        is_hovering_ = false;
        return true;
      }
    }
    return false;
  }
} }
