/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <unordered_map>
#include <vector>
#include "../common/volume.h"
#include "element.h"
namespace game { namespace ui
{
  struct Controller
  {
    virtual ~Controller() noexcept {}

    void clickable_region(Volume<int> const&, Element&) noexcept;
    void draggable_region(Volume<int> const&, Element&) noexcept;

    void clear_click_regions(Element&) noexcept;
    void clear_drag_regions(Element&) noexcept;
  private:
    // We'll see how this works for now. Each element has a given list of click
    // regions and drag regions.
    std::unordered_map<Element*, std::vector<Volume<int> > > click_;
    std::unordered_map<Element*, std::vector<Volume<int> > > drag_;
  };
} }
