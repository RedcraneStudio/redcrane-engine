/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "controller.h"
#include "element.h"
namespace game { namespace ui
{
  void Controller::clickable_region(Volume<int> const& v, Element& e) noexcept
  {
    click_[&e].push_back(v);
  }
  void Controller::draggable_region(Volume<int> const& v, Element& e) noexcept
  {
    drag_[&e].push_back(v);
  }

  void Controller::clear_click_regions(Element& e) noexcept
  {
    click_[&e].clear();
  }
  void Controller::clear_drag_regions(Element& e) noexcept
  {
    drag_[&e].clear();
  }
} }
