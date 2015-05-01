/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "bar.h"
namespace game { namespace ui
{
  Vec<int> Bar::get_minimum_extents_() const noexcept
  {
    return {0, 0};
  }
  Volume<int> Bar::layout_()
  {
    return this->parent_volume();
  }
  void Bar::render_(Renderer& r) const noexcept
  {
    auto vol = this_volume();

    if(max_ == 0)
    {
      // Make it totally empty when we don't have a maximum
      vol.width = 0;
    }
    else
    {
      // Reduce the parent volume by a certain amount.
      // Divide the width into equal segments.
      auto seg_size = (double) vol.width / max_;
      vol.width -= seg_size * (max_ - cur_);
    }

    r.set_draw_color(color_);
    r.fill_rect(vol);
  }
} }
