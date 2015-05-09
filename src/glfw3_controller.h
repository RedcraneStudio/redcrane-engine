/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "glfw3.h"
#include "ui/element.h"
#include "ui/mouse_logic.h"
#include "common/vec.h"
namespace game { namespace ui
{
  struct GLFW_Controller
  {
    GLFW_Controller(GLFWwindow* window) noexcept : window_(window) {}
    bool step(Shared_Element root, Mouse_State ns) noexcept;

    static Mouse_State cur_mouse(GLFWwindow* win) noexcept;
  private:
    GLFWwindow* window_;
    Mouse_State old_mouse_;
  };
} }
