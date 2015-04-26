/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <vector>
#include <memory>

#include "View.h"
namespace game { namespace ui
{
  struct null_layout_t {};

  template <class Layout_Type>
  struct View_Child
  {
    Shared_View view;
    Layout_Type layout;
  };

  template <class Layout_Type>
  struct View_Container : public View
  {
    View_Container(Graphics_Desc& g) noexcept : View(g) {}

    virtual bool is_container() const noexcept { return true; }

    using child_t = View_Child<Layout_Type>;
    using child_vec_t = std::vector<child_t>;

    inline void push_child(Shared_View,
                           Layout_Type = Layout_Type{}) noexcept;
    inline void push_child(child_t) noexcept;

    inline void insert_child(typename child_vec_t::iterator,
                             Shared_View,
                             Layout_Type = Layout_Type{}) noexcept;
    inline void insert_child(typename child_vec_t::const_iterator,
                             Shared_View,
                             Layout_Type = Layout_Type{}) noexcept;
    inline void insert_child(typename child_vec_t::iterator,
                             child_t) noexcept;
    inline void insert_child(typename child_vec_t::const_iterator,
                             child_t) noexcept;

    inline void remove_child(Shared_View) noexcept;
    inline void remove_child(typename child_vec_t::size_type) noexcept;
    inline void remove_child(typename child_vec_t::iterator) noexcept;
    inline void remove_child(typename child_vec_t::const_iterator) noexcept;

    void invalidate() noexcept override;

    std::vector<Shared_View> children() noexcept override;

  protected:
    child_vec_t children_;

  private:
    // Left to be implemented by any deriving classes.
    // Volume<int> layout_() override;
    // Vec<int> get_minimum_extents_() const noexcept override;

    inline bool dispatch_event_(SDL_Event const&) noexcept override;

    inline void render_() const noexcept override;
    Shared_View find_child_(std::string, bool) const noexcept override;
    bool replace_child_(std::string, Shared_View, bool) noexcept override;
  };
} }

#include "View_Container_impl.hpp"
