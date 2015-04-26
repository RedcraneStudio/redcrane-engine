/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "Mouse_Click.h"
namespace game { namespace ui
{
  bool Mouse_Click::
  try_trigger(View& v, SDL_Event const& event) const noexcept
  {
    if((event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)&&
       event.button.button == SDL_BUTTON_LEFT)
    {
      auto pt = Vec<int>{event.button.x, event.button.y};

      auto volume = Volume<int>{};
      if(use_parent_vol_) volume = v.parent_volume();
      else volume = v.this_volume();

      if(is_in(volume, pt))
      {
        if(event.type == SDL_MOUSEBUTTONDOWN)
        {
          if(on_click_) on_click_(pt);
        }
        else
        {
          if(on_unclick_) on_unclick_(pt);
        }
        return true;
      }
    }
    return false;
  }
} }
