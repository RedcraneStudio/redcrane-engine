/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "linear_layout.h"
#include <numeric>
namespace game { namespace ui
{
  Volume<int> Linear_Layout::layout_()
  {
    auto vol = parent_volume();

    // Record the data that we will need to change.
    int Vec<int>::* comp = nullptr;
    int Volume<int>::* extent = nullptr;

    // If horizontally aligning objects.
    if(orientation == Orient::Horizontal)
    {
      //.---------. <= Cell extent, component, and extent.
      //######################
      //#         #          #
      //######################

      comp = &Vec<int>::x;
      extent = &Volume<int>::width;
    }
    // If vertically aligning objects.
    else if(orientation == Orient::Vertical)
    {
      //###########-,
      //#         # | <= Cell extent, component, and extent.
      //###########-*
      //#         #
      //###########
      comp = &Vec<int>::y;
      extent = &Volume<int>::height;
    }
    else { /* I'm retarded? */ }

    using std::begin; using std::end;
    auto divisions = std::accumulate(begin(children_), end(children_), 0,
    [](int sum, auto const& child)
    {
      return sum + child.layout.weight;
    });

    // The cell extent is the length of the axis that is going to be needing
    // an adjustment.
    // For example, if the orientation is vertical, the height is going to need
    // to change, while the width will stay constant for each child's cell.
    int cell_extent = vol.*extent / divisions;

    for(unsigned int cur_weight = 0, child_i = 0;
        child_i < children_.size();
        ++child_i)
    {
      auto view_vol = vol;

      auto& child = children_[child_i];

      view_vol.*extent = cell_extent * child.layout.weight;
      view_vol.pos.*comp += cell_extent * cur_weight;

      // If this is the last child
      if(child_i == children_.size() - 1 && force_fill)
      {
        // Fill the rest of the volume, unconditionally. Doesn't seem to cause
        // problems so it is enabled by default.
        view_vol.*extent = (vol.pos.*comp + vol.*extent) - view_vol.pos.*comp;
      }

      cur_weight += child.layout.weight;

      child.view->layout(view_vol);
    }

    return vol;
  }

  // This implementation is currently broken.
  Vec<int> Linear_Layout::get_minimum_extents_() const noexcept
  {
    auto bounds = Vec<int>{};

    for(auto const& child : children_)
    {
      auto const& view_vec = child.view->get_minimum_extents();

      if(orientation == Orient::Vertical)
      {
        bounds.x = std::max(bounds.x, view_vec.x);
        bounds.y += view_vec.y;
      }
      else
      {
        bounds.x += view_vec.x;
        bounds.y += std::max(bounds.y, view_vec.y);
      }
    }

    return bounds;
  }
} }
