/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "glfw3_controller.h"
#include "ui/element_iterator.h"
namespace game { namespace ui
{
  void GLFW_Controller::step(Shared_Element root) noexcept
  {
    glfwPollEvents();

    Vec<double> np_double;
    glfwGetCursorPos(window_, &np_double.x, &np_double.y);

    auto new_mouse =
      Mouse_State{glfwGetMouseButton(window_,
                                     GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS,
                  vec_cast<int>(np_double)};

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
          }
          if(is_hover(new_mouse, old_mouse_))
          {
            cur_elem.on_hover(pt);
          }
          if(is_drag(new_mouse, old_mouse_))
          {
            cur_elem.on_drag(new_mouse.position, old_mouse_.position);
          }
        }
      }
    }

    old_mouse_ = new_mouse;
  }
} }
