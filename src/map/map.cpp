/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "map.h"

namespace game
{
  Structure_Instance::Structure_Instance(Structure const& s, pos_t p) noexcept
    : position(p), s_type_(&s){}

  void Structure_Instance::set_structure_type(Structure const& s) noexcept
  {
    s_type_ = &s;
  }
  Structure const& Structure_Instance::structure() const noexcept
  {
    return *s_type_;
  }

  bool try_structure_place(Map& map, Structure const& st, pos_t pos) noexcept
  {
    // Check collision with every other structure.

    // Make sure it is in the bounds of the map

    // Any other constraints?

    map.structures.emplace_back(st, pos);

    return true;
  }
}
