/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "pie_menu.h"
namespace game { namespace ui
{
  void Pie_Menu::center_button_behavior(Center_Behavior cb) noexcept
  {
    cb_behavior_ = cb;
  }
  Center_Behavior Pie_Menu::center_button_behavior() const noexcept
  {
    return cb_behavior_;
  }

  void Pie_Menu::center_button(pie_button_t pb) noexcept
  {
    center_button_ = pb;
  }
  pie_button_t Pie_Menu::center_button() const noexcept
  {
    return center_button_;
  }

  void Pie_Menu::num_buttons(std::size_t new_size) noexcept
  {
    radial_buttons_.resize(new_size);
  }
  std::size_t Pie_Menu::num_buttons() const noexcept
  {
    return radial_buttons_.size();
  }

  void Pie_Menu::radial_button(std::size_t index, pie_button_t pb) noexcept
  {
    radial_buttons_[index] = pb;
  }
  pie_button_t Pie_Menu::radial_button(std::size_t index) const noexcept
  {
    return radial_buttons_[index];
  }

  void Pie_Menu::handle_event(Mouse_State ms) noexcept
  {
    // Check if the mouse is hovering over us.
  }
  void Pie_Menu::render(Renderer& r) const noexcept
  {
    r.set_draw_color(Color{0x3d, 0x5c, 0x6d, 0xaa});
    r.fill_circle(center_, radius_, 35);

    // Render inside circle
    r.set_draw_color(Color{0x00, 0x00, 0x00, 0xaa});
    r.draw_circle(center_, radius_ / 3.5f, 35);

    // Render lines for each pie button in the radial buttons vector.
    auto segment_angle = 2 * M_PI / radial_buttons_.size();
    for(std::size_t i = 0; i < radial_buttons_.size(); ++i)
    {
      //auto angle = ((M_PI / 2) + segment_angle / 2) + (segment_angle * i);
      auto angle = segment_angle * i;
      // TODO: Figure out why exactly we have to negate sin in this case.
      Vec<float> dir{glm::cos(angle), -glm::sin(angle)};
      dir = normalize(dir);

      auto p1 = dir * (float) (radius_ / 3.5f + 10.0f) + center_;
      auto p2 = dir * (float) (radius_ - 10) + center_;
      r.draw_line(p1, p2);
    }

    // TODO: Render labels.
  }
} }
