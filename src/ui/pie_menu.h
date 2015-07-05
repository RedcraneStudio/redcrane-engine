/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <vector>
#include <string>
#include "mouse_logic.h"
#include "renderer.h"
namespace game { namespace ui
{
  enum class Center_Behavior
  {
    None,
    Last_Selected,
  };

  using pie_button_t = std::string;

  // We are purposely separating this implementation from the Element
  // hierarchy because we want to handle events differently. Basically, for a
  // pie menu, there is only one way to handle input. Especially since we have
  // abstracted mouse state with Mouse_State.
  struct Pie_Menu
  {
    void center_button_behavior(Center_Behavior cb) noexcept;
    Center_Behavior center_button_behavior() const noexcept;

    void center_button(pie_button_t) noexcept;
    pie_button_t center_button() const noexcept;

    void num_buttons(std::size_t) noexcept;
    std::size_t num_buttons() const noexcept;

    void radial_button(std::size_t index, pie_button_t) noexcept;
    pie_button_t radial_button(std::size_t index) const noexcept;

    void radius(int rad_pix) noexcept { radius_ = rad_pix; }
    int  radius() const noexcept { return radius_; }

    void     center(Vec<int> cent) noexcept { center_ = cent; }
    Vec<int> center() const noexcept { return center_; }

    void handle_event(Mouse_State ms) noexcept;

    void render(Renderer&) const noexcept;
  private:
    int radius_;
    Vec<int> center_;

    Center_Behavior cb_behavior_;

    // Replace this with a maybe owned, maybe, so we can possible reference an
    // existing button that comes from the radial buttons vector and not waste
    // memory or risk one or the other going out of date, etc.
    pie_button_t center_button_;
    std::vector<pie_button_t> radial_buttons_;
  };
} }
