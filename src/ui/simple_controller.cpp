/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "simple_controller.h"
#include "element_iterator.h"
namespace redc { namespace ui
{
  bool Simple_Controller::step(Shared_Element root,
                               Mouse_State mouse_state) noexcept
  {
    bool ret = false;

    auto iter = D_F_Elem_Iter{root};

    for(; !iter.is_end(); ++iter)
    {
      auto& cur_elem = *iter;
      if(cur_elem.handle_events())
      {
        if(is_in(cur_elem.this_volume(), mouse_state.position))
        {
          cur_elem.step_mouse(mouse_state);
          ret = true;
        }
      }
    }

    return ret;
  }
} }
