/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../../element_composite.h"

#include <boost/optional.hpp>
namespace redc { namespace ui
{
  struct Grid_Layout_Params
  {
    int row = 0;
    int col = 0;
  };

  struct Grid_Layout : public Element_Composite<Grid_Layout_Params>
  {
    bool force_fill_width = true;
    bool force_fill_height = true;

  private:
    inline Vec<int> get_minimum_extents_() const noexcept override
    { return {0,0}; }

    Volume<int> layout_() override;
  };
} }
