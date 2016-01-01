/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../../element_composite.h"
namespace redc { namespace ui
{
  struct Linear_Layout_Params
  {
    int weight = 1;
  };

  struct Linear_Layout : public Element_Composite<Linear_Layout_Params>
  {
    enum class Orient
    { Vertical, Horizontal };

    Orient orientation;
    bool force_fill = true;

  private:
    Vec<int> get_minimum_extents_() const noexcept override;
    Volume<int> layout_() override;
  };
} }
