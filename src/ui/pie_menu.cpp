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

  void Pie_Menu::center_button(std::string pb) noexcept
  {
    center_button_.str(pb);
  }
  std::string Pie_Menu::center_button() const noexcept
  {
    return center_button_.str();
  }

  void Pie_Menu::num_buttons(std::size_t new_size) noexcept
  {
    auto old_size = radial_buttons_.size();
    radial_buttons_.resize(new_size);

    // Update font renderer.
    auto dif = new_size - old_size;
    for(int i = 0; i < dif; ++i)
    {
      radial_buttons_[i + old_size].font_renderer(*font_);
    }
  }
  std::size_t Pie_Menu::num_buttons() const noexcept
  {
    return radial_buttons_.size();
  }

  void Pie_Menu::radial_button(std::size_t index, std::string pb) noexcept
  {
    radial_buttons_[index].str(pb);
  }
  std::string Pie_Menu::radial_button(std::size_t index) const noexcept
  {
    return radial_buttons_[index].str();
  }

  void Pie_Menu::font_renderer(IFont_Renderer& f) noexcept
  {
    // New font renderer? Update labels.
    bool update_labels = &f != font_;

    font_ = &f;

    if(update_labels)
    {
      for(auto& label : radial_buttons_)
      {
        label.font_renderer(*font_);
      }
      center_button_.font_renderer(*font_);
    }
  }
  IFont_Renderer& Pie_Menu::font_renderer() const noexcept
  {
    return *font_;
  }

  void Pie_Menu::handle_event(Mouse_State ms) noexcept
  {
    // Check if the mouse is hovering over us.
  }
  void Pie_Menu::render(Renderer& r) const noexcept
  {
    r.set_draw_color(Color{0x3d, 0x5c, 0x6d, 0xaa});
    r.fill_circle({center_, radius_}, 35);

    // Render inside circle

    auto radius_factor = 3.5f;

    r.set_draw_color(Color{0x00, 0x00, 0x00, 0xaa});
    r.draw_circle({center_, static_cast<int>(radius_ / radius_factor)}, 35);

    // Render lines for each pie button in the radial buttons vector.
    auto segment_angle = 2 * M_PI / radial_buttons_.size();

    for(std::size_t i = 0; i < radial_buttons_.size(); ++i)
    {
      auto angle = ((M_PI / 2) - segment_angle / 2) + (segment_angle * i);

      // TODO: Figure out why exactly we have to negate sin in this case.
      Vec<float> dir{glm::cos(angle), -glm::sin(angle)};
      dir = normalize(dir);

      auto p1 = dir * (float) (radius_ / radius_factor + 10.0f) + center_;
      auto p2 = dir * (float) (radius_ - 10) + center_;
      r.draw_line(p1, p2);
    }

    // ratio = width / height
    auto calculate_bounds = [&](Vec<float> pt, int size, float ratio)
    {
      // TODO: Make this calculation configurable.
      // Consider size to be the length of whatever radius we have. (small/big)

      // Make it just a teensy bit smaller.
      auto max_size = size * 3 / 4;

      Volume<float> ret;

      ret.pos.x = pt.x - max_size / 2;
      ret.pos.y = pt.y - max_size / 2 / ratio;
      ret.width = max_size;
      ret.height = max_size / ratio;

      return ret;
    };

    for(std::size_t i = 0; i < radial_buttons_.size(); ++i)
    {
      auto angle = (M_PI / 2) + (segment_angle * i);

      Vec<float> dir{glm::cos(angle), -glm::sin(angle)};
      dir = normalize(dir);

      // The result of factoring `(rad - sr) / 2 + sr` where rad is radius, sr,
      // or small radius, is `rad / factor`.
      auto ln_to_center = (radius_factor * radius_ + radius_) /
                          (2 * radius_factor);
      auto pt = dir * ln_to_center + center_;

      // Expand from that point.
      auto long_rad = radius_ - radius_ / radius_factor;

      // Get the volume:
      // TODO: Make the size configurable.
      auto vol = calculate_bounds(pt, long_rad, 1.75f);
      if(radial_buttons_[i].parent_volume() != volume_cast<int>(vol))
      {
        // TODO: Cache whether or not we layed the label's out?
        radial_buttons_[i].layout(volume_cast<int>(vol));
      }

      // TODO: Make color configurable.
      radial_buttons_[i].color(colors::white);
      radial_buttons_[i].render(r);
    }

    auto vol = calculate_bounds(center_, radius_ / radius_factor, 1.0f);
    if(center_button_.parent_volume() != volume_cast<int>(vol))
    {
      center_button_.layout(volume_cast<int>(vol));
    }
    center_button_.color(colors::white);
    center_button_.render(r);
  }
} }
