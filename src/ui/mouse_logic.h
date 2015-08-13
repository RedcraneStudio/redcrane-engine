/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 *
 * \file mouse_logic.h This file provides a few helper functions for
 * controllers.
 */
#pragma once
#include "../common/vec.h"
#include <functional>
namespace game { namespace ui
{
  enum Mouse_Button
  {
    Mouse_Button_Left = 0x01, Mouse_Button_Middle = 0x02,
    Mouse_Button_Right = 0x04
  };

  struct Mouse_State
  {
    unsigned int buttons = 0x00;
    Vec<int> position;
    double scroll_delta = 0.0f;
  };

  using mouse_state_fn_t = void (Mouse_State const&);
  using mouse_state_cb_t = std::function<mouse_state_fn_t>;

  struct On_Click_Handler
  {
    On_Click_Handler(mouse_state_cb_t const& cb,
                     Mouse_Button button = Mouse_Button_Left) noexcept
                     : cb_(cb), button_(button) {}

    void operator()(Mouse_State const&) noexcept;
  private:
    mouse_state_cb_t cb_;
    Mouse_Button button_;
  };
  struct On_Release_Handler
  {
    On_Release_Handler(mouse_state_cb_t const& cb,
                       Mouse_Button button = Mouse_Button_Left) noexcept
                       : cb_(cb), button_(button) {}

    void operator()(Mouse_State const&) noexcept;
  private:
    mouse_state_cb_t cb_;

    Mouse_Button button_;
    bool clicked_;
  };
} }
