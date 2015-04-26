/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../View_Container.h"
namespace game { namespace ui
{
  struct Padding
  {
    int left = 0;
    int right = 0;
    int top = 0;
    int bottom = 0;
  };

  enum class Horizontal_Alignment
  {
    Left, Center, Right
  };
  enum class Vertical_Alignment
  {
    Top, Center, Bottom
  };

  struct Alignment
  {
    Horizontal_Alignment horizontal;
    Vertical_Alignment vertical;
  };
  struct Side_Layout_Params
  {
    Padding padding;
    Alignment alignment;
  };

  struct Side_Layout : View_Container<Side_Layout_Params>
  {
    explicit Side_Layout(Graphics_Desc& g) noexcept : View_Container(g) {}

  private:
    Vec<int> get_minimum_extents_() const noexcept override;
    Volume<int> layout_() override;
  };
} }
