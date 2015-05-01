/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <boost/optional.hpp>

#include "../common/volume.h"
#include "../common/vec.h"
#include "../common/color.h"

#include "cast.h"

#include "controller.h"
#include "renderer.h"
namespace game { namespace ui
{
  struct Element;
  using Model = Element;

  using Shared_Element = std::shared_ptr<Element>;

  struct Small_Volume_Error {};
  enum class Elem_Volume
  {
    This, Parent
  };

  // Pronouned: ui::Element. Our ui element here is our model. The exact UI
  // isn't up to much interpretation, since positions are going to be worked
  // out based on the layout type before rendering and independent of the view
  // implementation. Speaking of, the view implementation is more about how
  // the rendering get's done. In other words: OpenGL v. SDL v. DirectX, etc.
  struct Element
  {
    virtual ~Element() {}

    virtual bool is_container() const noexcept { return false; }

    inline bool layout(Vec<int>);
    inline bool layout(Volume<int>);

    Vec<int> get_minimum_extents() const noexcept;

    inline Volume<int> const& parent_volume() const noexcept;
    inline Volume<int> const& this_volume() const noexcept;

    std::string id;

    void set_border(Elem_Volume, Color color) noexcept;
    boost::optional<Color> query_border(Elem_Volume) const noexcept;
    bool remove_border(Elem_Volume) noexcept;

    void set_background(Elem_Volume, Color color) noexcept;
    boost::optional<Color> query_background(Elem_Volume) const noexcept;
    bool remove_background(Elem_Volume) noexcept;

    inline void visible(bool visible) noexcept;
    inline bool visible() const noexcept;

    inline void handle_events(bool h) noexcept;
    inline bool handle_events() noexcept;

    inline Vec<int> min_size() const noexcept;
    inline void min_size(Vec<int>) noexcept;

    inline Shared_Element
    find_child(std::string id, bool recursive = false) const noexcept;

    inline Shared_Element
    find_child_r(std::string id, bool recursive = true) const noexcept;

    template <class T> inline std::shared_ptr<T>
    find_child(std::string, bool = false) const noexcept;

    template <class T> inline std::shared_ptr<T>
    find_child_r(std::string, bool = true) const noexcept;

    inline bool
    replace_child(std::string, Shared_Element, bool r = false) noexcept;

    inline bool
    replace_child_r(std::string, Shared_Element, bool r = true) noexcept;

    inline void render(Renderer&) const noexcept;
    inline void activate_regions(Controller&) const noexcept;

    /*!
     * \brief This is used to set a single child *without layout params*.
     *
     * It can be set for non-composite elements to get fairly good buttons for
     * example with pre-existing elements. A button element handles the
     * controller, it's child a sprite handles the button graphic and a label
     * child can handle the text, for example.
     */
    virtual void set_child(std::weak_ptr<Element> e) noexcept;
    inline virtual void remove_child() noexcept { child_.reset(); }
  protected:
    Volume<int> parent_vol_;
    Volume<int> this_vol_;

    bool layed_out_ = false;
    bool visible_ = true;
    bool handle_events_ = true;

    virtual Volume<int> layout_() = 0;
    virtual Vec<int> get_minimum_extents_() const noexcept = 0;

    boost::optional<Color> this_border_;
    boost::optional<Color> parent_border_;

    boost::optional<Color> this_background_;
    boost::optional<Color> parent_background_;

    Vec<int> min_size_;

    inline virtual Shared_Element
    find_child_(std::string, bool) const noexcept;

    inline virtual bool
    replace_child_(std::string, Shared_Element, bool) noexcept;

    virtual void render_(Renderer&) const noexcept = 0;
    virtual void activate_regions_(Controller&) const noexcept = 0;

    std::weak_ptr<Element> child_;
  };

  inline Volume<int> const& Element::parent_volume() const noexcept
  {
    return parent_vol_;
  }
  inline Volume<int> const& Element::this_volume() const noexcept
  {
    return this_vol_;
  }

  inline void Element::visible(bool visible) noexcept
  {
    visible_ = visible;
  }
  inline bool Element::visible() const noexcept
  {
    return visible_;
  }
  inline void Element::handle_events(bool h) noexcept
  {
    handle_events_ = h;
  }
  inline bool Element::handle_events() noexcept
  {
    return handle_events_;
  }

  inline bool Element::layout(Vec<int> size)
  {
    return layout({{0, 0}, size.x, size.y});
  }
  inline bool Element::layout(Volume<int> vol)
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

    auto child_ptr = child_.lock();
    if(child_ptr) child_ptr->layout(vol);

    return true;
  }

  inline Vec<int> Element::min_size() const noexcept
  {
    return min_size_;
  }
  inline void Element::min_size(Vec<int> ms) noexcept
  {
    min_size_ = ms;
  }

  inline Shared_Element
  Element::find_child(std::string id, bool r) const noexcept
  {
    return find_child_(id, r);
  }
  inline Shared_Element
  Element::find_child_r(std::string id, bool r) const noexcept
  {
    return find_child_(id, r);
  }


  template <class T> inline std::shared_ptr<T>
  Element::find_child(std::string id, bool r) const noexcept
  {
    return as<T>(find_child(id, r));
  }
  template <class T> inline std::shared_ptr<T>
  Element::find_child_r(std::string id, bool r) const noexcept
  {
    return as<T>(find_child(id, r));
  }

  inline bool
  Element::replace_child(std::string i, Shared_Element v, bool r) noexcept
  {
    return replace_child_(i, v, r);
  }

  inline bool
  Element::replace_child_r(std::string i, Shared_Element v, bool r) noexcept
  {
    return replace_child_(i, v, r);
  }

  // Default impls of find_child_ and replace_child_ don't do anything.

  inline Shared_Element Element::find_child_(std::string, bool) const noexcept
  {
    return nullptr;
  }
  inline bool
  Element::replace_child_(std::string, Shared_Element, bool) noexcept
  {
    return false;
  }

  inline void Element::render(Renderer& r) const noexcept
  {
    // TODO render border and background color.
    if(layed_out_ && visible_) render_(r);

    // Render the child.
    auto child_ptr = child_.lock();
    if(child_ptr) child_ptr->render(r);
  }
  inline void Element::activate_regions(Controller& c) const noexcept
  {
    if(layed_out_ && visible_ && handle_events_) activate_regions_(c);

    // Tell the child.
    auto child_ptr = child_.lock();
    if(child_ptr) child_ptr->activate_regions(c);
  }
  void Element::set_child(std::weak_ptr<Element> e) noexcept
  {
    child_ = e;
  }
} }
