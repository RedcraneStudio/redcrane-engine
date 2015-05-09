/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "glfw3_controller.h"
#include "ui/element_iterator.h"
namespace game { namespace ui
{
  bool GLFW_Controller::step(Shared_Element root,
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

    old_mouse_ = new_mouse;

    return ret;
  }
  Mouse_State GLFW_Controller::cur_mouse(GLFWwindow* win) noexcept
  {
    Vec<double> np_double;
    glfwGetCursorPos(win, &np_double.x, &np_double.y);

    auto ret =
      Mouse_State{glfwGetMouseButton(win,GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS,
                  vec_cast<int>(np_double)};
    return ret;
  }
} }
