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
}
// Forward declare this stuff.
namespace gfx { struct IDriver; struct Camera; }
namespace ui
{
  // Requires first person mode in camera. TODO Assert this in code.
  struct Camera_Rotation_Helper
  {
    // Speed is in radians/pixel of mouse movement, I think.
    Camera_Rotation_Helper(gfx::IDriver& d, gfx::Camera& c,
                           float speed = .001f,
                           Mouse_Button btn = Mouse_Button_Right) noexcept
                           : driver_(d), camera_(c), speed_(speed), btn_(btn){}
    void step_mouse(Mouse_State const& ms) noexcept;
  private:
    gfx::IDriver& driver_;
    gfx::Camera& camera_;
    float speed_;
    Mouse_Button btn_;

    bool clicked_;

    Vec<float> prev_pos_;
    Vec<float> map_pos_;
  };

  struct Camera_Orientation_Helper
  {
    explicit Camera_Orientation_Helper(gfx::Camera& c, float x_speed = .001,
                                       float y_speed = .001,
                                       Mouse_Button btn = Mouse_Button_Middle)
                                       noexcept : camera_(c), btn_(btn),
                                      x_speed_(x_speed), y_speed_(y_speed) {}
    void step_mouse(Mouse_State const& ms) noexcept;

    void reset() noexcept { cur_state_ = Cur_State::Start; }
    bool done() noexcept { return cur_state_ == Cur_State::Done; }
  private:
    gfx::Camera& camera_;
    Mouse_Button btn_;

    enum class Cur_State
    {
      Start, Clicked, Released, Almost_Done, Done
    } cur_state_ = Cur_State::Start;

    float x_speed_;
    float y_speed_;

    Vec<float> old_pos_;
  };
} }
