/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "element.h"
#include "mouse_logic.h"
#include "../common/vec.h"
#include <boost/signals2.hpp>
namespace game { namespace ui
{
  namespace signals2 = boost::signals2;

  struct Simple_Controller
  {
    bool step(Shared_Element root, Mouse_State ns) noexcept;

    using click_signal_t = signals2::signal<void (Vec<int>) >;

    signals2::connection
    add_click_listener(click_signal_t::slot_type s) noexcept
    { return on_click_.connect(s); }

    using hover_signal_t = signals2::signal<void (Vec<int>) >;

    signals2::connection
    add_hover_listener(hover_signal_t::slot_type s) noexcept
    { return on_hover_.connect(s); }

    using drag_signal_t = signals2::signal<void (Vec<int>, Vec<int>) >;

    signals2::connection add_drag_listener(drag_signal_t::slot_type s) noexcept
    { return on_drag_.connect(s); }
  private:
    Mouse_State old_mouse_;

    click_signal_t on_click_;
    hover_signal_t on_hover_;
    drag_signal_t on_drag_;
  };
} }
