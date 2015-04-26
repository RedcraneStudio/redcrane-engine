/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "View.h"
#include "../common/render.h"
namespace game { namespace ui
{
  void View::render() const noexcept
  {
    if(layed_out_ && visible_)
    {
      // Render background
      if(parent_background_)
      {
        fill_volume(graphics_.renderer, parent_volume(),
                    parent_background_.value());
      }
      if(this_background_)
      {
        fill_volume(graphics_.renderer, this_volume(),
                    this_background_.value());
      }

      // Render the view.
      render_();

      // Render borders
      if(parent_border_)
      {
        draw_volume(graphics_.renderer, parent_volume(),
                    parent_border_.value());
      }
      if(this_border_)
      {
        draw_volume(graphics_.renderer, this_volume(),
                    this_border_.value());
      }
    }
  }

  Vec<int> View::get_minimum_extents() const noexcept
  {
    auto min = this->get_minimum_extents_();
    return {std::max(min.x, min_size_.x), std::max(min.y, min_size_.y)};
  }

  void View::set_border(View_Volume vol, Color color) noexcept
  {
    switch(vol)
    {
      case View_Volume::This:
      {
        this_border_ = color;
      }
      case View_Volume::Parent:
      {
        parent_border_ = color;
      }
    }
  }
  boost::optional<Color> View::query_border(View_Volume v) const noexcept
  {
    switch(v)
    {
      case View_Volume::This:
      {
        return this_border_;
      }
      case View_Volume::Parent:
      {
        return parent_border_;
      }
    }
  }
  bool View::remove_border(View_Volume v) noexcept
  {
    auto& border =
      v == View_Volume::This ?  this_border_ : parent_border_;

    if(border)
    {
      border = boost::none;
      return true;
    }
    return false;
  }

  void View::set_background(View_Volume v, Color color) noexcept
  {
    switch(v)
    {
      case View_Volume::This:
      {
        this_background_ = color;
        break;
      }
      case View_Volume::Parent:
      {
        parent_background_ = color;
        break;
      }
    }
  }
  boost::optional<Color> View::query_background(View_Volume v) const noexcept
  {
    switch(v)
    {
      case View_Volume::This:
      {
        return this_background_;
      }
      case View_Volume::Parent:
      {
        return parent_background_;
      }
    }
  }
  bool View::remove_background(View_Volume v) noexcept
  {
    auto& background =
      v == View_Volume::This ?  this_background_ : parent_background_;

    if(background)
    {
      background = boost::none;
      return true;
    }
    return false;
  }

  Vec<int> View::min_size() const noexcept
  {
    return min_size_;
  }
  void View::min_size(Vec<int> s) noexcept
  {
    min_size_ = s;
  }
} }
