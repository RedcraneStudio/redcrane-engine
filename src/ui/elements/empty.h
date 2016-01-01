/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../element.h"
namespace redc { namespace ui
{
  /*!
   * \brief A view that simply takes up space.
   */
  struct Empty : public Element
  {
    inline Vec<int> get_minimum_extents_() const noexcept override;
    inline Volume<int> layout_() override;
    void render_(Renderer&) const noexcept override {}
  };
  inline Vec<int> Empty::get_minimum_extents_() const noexcept
  {
    return {0, 0};
  }
  inline Volume<int> Empty::layout_()
  {
    return parent_volume();
  }
} }
