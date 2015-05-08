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

  Volume<int> const& Element::parent_volume() const noexcept
  {
    return parent_vol_;
  }
  Volume<int> const& Element::this_volume() const noexcept
  {
    return this_vol_;
  }

  void Element::visible(bool visible) noexcept
  {
    visible_ = visible;
  }
  bool Element::visible() const noexcept
  {
    return visible_;
  }
  void Element::handle_events(bool h) noexcept
  {
    handle_events_ = h;
  }
  bool Element::handle_events() noexcept
  {
    return handle_events_;
  }

  bool Element::layout(Vec<int> size)
  {
    return layout({{0, 0}, size.x, size.y});
  }
  bool Element::layout(Volume<int> vol)
  {
    try
    {
      parent_vol_ = std::move(vol);
      this_vol_ = layout_();
      layed_out_ = true;
    }
    catch(Small_Volume_Error& e)
    {
      return false;
    }
    catch(...)
    {
      throw;
    }

    return true;
  }

  Vec<int> Element::min_size() const noexcept
  {
    return min_size_;
  }
  void Element::min_size(Vec<int> ms) noexcept
  {
    min_size_ = ms;
  }

  Shared_Element Element::find_child(std::string id, bool r) const noexcept
  {
    return find_child_(id, r);
  }
  Shared_Element Element::find_child_r(std::string id, bool r) const noexcept
  {
    return find_child_(id, r);
  }


  template <class T>
  std::shared_ptr<T> Element::find_child(std::string id, bool r) const noexcept
  {
    return as<T>(find_child(id, r));
  }
  template <class T> std::shared_ptr<T>
  Element::find_child_r(std::string id, bool r) const noexcept
  {
    return as<T>(find_child(id, r));
  }

  bool Element::replace_child(std::string i, Shared_Element v, bool r) noexcept
  {
    return replace_child_(i, v, r);
  }

  bool
  Element::replace_child_r(std::string i, Shared_Element v, bool r) noexcept
  {
    return replace_child_(i, v, r);
  }

  void Element::render(Renderer& r) const noexcept
  {
    // TODO render border and background color.
    if(layed_out_ && visible_) render_(r);
  }
} }
