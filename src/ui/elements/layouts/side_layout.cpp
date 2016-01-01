/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "side_layout.h"
#include "../../../common/center.hpp"
namespace redc { namespace ui
{
    Vec<int> Side_Layout::get_minimum_extents_() const noexcept
    {
      return {0, 0};
    }
    Volume<int> Side_Layout::layout_()
    {
      auto vol = parent_volume();

      for(auto& child : children_)
      {
        auto child_vol = vol_from_extents(child.view->get_minimum_extents());

        switch(child.layout.alignment.vertical)
        {
          case Vertical_Alignment::Top:
          {
            // Use top padding.
            child_vol.pos.y = child.layout.padding.top;
            break;
          }
          case Vertical_Alignment::Center:
          {
            child_vol.pos.y = center(vol.pos.y, vol.height, child_vol.height);
            break;
          }
          case Vertical_Alignment::Bottom:
          {
            child_vol.pos.y = vol.height - child_vol.height -
                              child.layout.padding.bottom;
            break;
          }
        }
        switch(child.layout.alignment.horizontal)
        {
          case Horizontal_Alignment::Left:
          {
            // Use top padding.
            child_vol.pos.x = child.layout.padding.left;
            break;
          }
          case Horizontal_Alignment::Center:
          {
            child_vol.pos.x = center(vol.pos.x, vol.width, child_vol.width);
            break;
          }
          case Horizontal_Alignment::Right:
          {
            child_vol.pos.x = vol.width - child_vol.width -
                              child.layout.padding.right;
            break;
          }
        }

        child.view->layout(child_vol);
      }

      return vol;
    }
} }
