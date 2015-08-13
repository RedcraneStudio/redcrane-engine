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

#include "mouse_logic.h"
#include "cast.h"
#include "renderer.h"

#include <boost/signals2.hpp>

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

  namespace signals2 = boost::signals2;

  // Pronouned: ui::Element. Our ui element here is our model. The exact UI
  // isn't up to much interpretation, since positions are going to be worked
  // out based on the layout type before rendering and independent of the view
  // implementation. Speaking of, the view implementation is more about how
  // the rendering get's done. In other words: OpenGL v. SDL v. DirectX, etc.
  struct Element
  {
    Element() = default;

    Element(Element const&) = delete;
    Element(Element&&) = default;

    Element& operator=(Element const&) = delete;
    Element& operator=(Element&&) = default;

    virtual ~Element() {}

    virtual bool is_container() const noexcept { return false; }

    bool layout(Vec<int>);
    bool layout(Volume<int>);

    Vec<int> get_minimum_extents() const noexcept;

    Volume<int> const& parent_volume() const noexcept;
    Volume<int> const& this_volume() const noexcept;

    std::string id;

    void set_border(Elem_Volume, Color color) noexcept;
    boost::optional<Color> query_border(Elem_Volume) const noexcept;
    bool remove_border(Elem_Volume) noexcept;

    void set_background(Elem_Volume, Color color) noexcept;
    boost::optional<Color> query_background(Elem_Volume) const noexcept;
    bool remove_background(Elem_Volume) noexcept;

    void visible(bool visible) noexcept;
    bool visible() const noexcept;

    void handle_events(bool h) noexcept;
    bool handle_events() noexcept;

    Vec<int> min_size() const noexcept;
    void min_size(Vec<int>) noexcept;

    Shared_Element
    find_child(std::string id, bool recursive = false) const noexcept;

    Shared_Element
    find_child_r(std::string id, bool recursive = true) const noexcept;

    template <class T> std::shared_ptr<T>
    find_child(std::string, bool = false) const noexcept;

    template <class T> std::shared_ptr<T>
    find_child_r(std::string, bool = true) const noexcept;

    bool
    replace_child(std::string, Shared_Element, bool r = false) noexcept;

    bool
    replace_child_r(std::string, Shared_Element, bool r = true) noexcept;

    std::size_t child_count() const noexcept { return child_count_(); }
    Shared_Element child_at(std::size_t index) noexcept
    { return child_at_(index); }

    template <class T> std::shared_ptr<T> child_at(std::size_t index) noexcept
    { return as<T>(child_at_(index)); }

    void render(Renderer&) const noexcept;

    // Event handling interface.
    void step_mouse(Mouse_State const& ms) noexcept
    { if(handle_events_) step_mouse_(ms); }

    using mouse_signal_t = signals2::signal<mouse_state_fn_t>;

    signals2::connection
    on_step_mouse(mouse_signal_t::slot_type s) noexcept
    { return step_mouse_.connect(s); }
  protected:
    Volume<int> parent_vol_;
    Volume<int> this_vol_;

    bool layed_out_ = false;
    bool visible_ = true;
    bool handle_events_ = false;

    virtual Volume<int> layout_() = 0;
    virtual Vec<int> get_minimum_extents_() const noexcept = 0;

    boost::optional<Color> this_border_;
    boost::optional<Color> parent_border_;

    boost::optional<Color> this_background_;
    boost::optional<Color> parent_background_;

    Vec<int> min_size_;

    // child query functions. Implemented as-if we are an empty composite by
    // default.
    virtual Shared_Element find_child_(std::string, bool) const noexcept
    { return nullptr; }
    virtual bool replace_child_(std::string, Shared_Element, bool) noexcept
    { return false; }
    virtual std::size_t child_count_() const noexcept
    { return 0; }
    virtual Shared_Element child_at_(std::size_t) noexcept
    { return nullptr; }

    virtual void render_(Renderer&) const noexcept = 0;

    mouse_signal_t step_mouse_;
  };
} }
