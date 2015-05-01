/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "element.h"
namespace game { namespace ui
{
#if 0
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
#endif
  Vec<int> Element::get_minimum_extents() const noexcept
  {
    auto min = this->get_minimum_extents_();
    return {std::max(min.x, min_size_.x), std::max(min.y, min_size_.y)};
  }

  void Element::set_border(Elem_Volume vol, Color color) noexcept
  {
    switch(vol)
    {
      case Elem_Volume::This:
      {
        this_border_ = color;
      }
      case Elem_Volume::Parent:
      {
        parent_border_ = color;
      }
    }
  }
  boost::optional<Color> Element::query_border(Elem_Volume v) const noexcept
  {
    switch(v)
    {
      case Elem_Volume::This:
      {
        return this_border_;
      }
      case Elem_Volume::Parent:
      {
        return parent_border_;
      }
    }
  }
  bool Element::remove_border(Elem_Volume v) noexcept
  {
    auto& border =
      v == Elem_Volume::This ?  this_border_ : parent_border_;

    if(border)
    {
      border = boost::none;
      return true;
    }
    return false;
  }

  void Element::set_background(Elem_Volume v, Color color) noexcept
  {
    switch(v)
    {
      case Elem_Volume::This:
      {
        this_background_ = color;
        break;
      }
      case Elem_Volume::Parent:
      {
        parent_background_ = color;
        break;
      }
    }
  }
  boost::optional<Color>
  Element::query_background(Elem_Volume v) const noexcept
  {
    switch(v)
    {
      case Elem_Volume::This:
      {
        return this_background_;
      }
      case Elem_Volume::Parent:
      {
        return parent_background_;
      }
    }
  }
  bool Element::remove_background(Elem_Volume v) noexcept
  {
    auto& background =
      v == Elem_Volume::This ?  this_background_ : parent_background_;

    if(background)
    {
      background = boost::none;
      return true;
    }
    return false;
  }
} }
