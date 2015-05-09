/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "simple_controller.h"
#include "element_iterator.h"
namespace game { namespace ui
{
  bool Simple_Controller::step(Shared_Element root,
                               Mouse_State new_mouse) noexcept
  {
    bool ret = false;

    auto iter = D_F_Elem_Iter{root};

    for(; !iter.is_end(); ++iter)
    {
      auto& cur_elem = *iter;
      if(cur_elem.handle_events())
      {
        auto pt = new_mouse.position;
        if(is_in(cur_elem.this_volume(), pt))
        {
          if(is_click(new_mouse, old_mouse_))
          {
            cur_elem.on_click(pt);
            ret = true;
          }
          if(is_hover(new_mouse, old_mouse_))
          {
            cur_elem.on_hover(pt);
            ret = true;
          }
          if(is_drag(new_mouse, old_mouse_))
          {
            cur_elem.on_drag(new_mouse.position, old_mouse_.position);
            ret = true;
          }
        }
      }
    }

    if(ret == false)
    {
      if(is_click(new_mouse, old_mouse_))
      {
        on_click_(new_mouse.position);
      }
      if(is_hover(new_mouse, old_mouse_))
      {
        on_hover_(new_mouse.position);
      }
      if(is_drag(new_mouse, old_mouse_))
      {
        on_drag_(new_mouse.position, old_mouse_.position);
      }
    }

    old_mouse_ = new_mouse;

    return ret;
  }
} }
