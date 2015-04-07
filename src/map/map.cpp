/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "map.h"
namespace strat
{
  Terrain make_flat_terrain(int alt, int w, int h)
  {
    auto terrain = Terrain{};
    for(int i = 0; i < h; ++i)
    {
      terrain.altitude.emplace_back();
      for(int j = 0; j < w; ++j)
      {
        terrain.altitude.back().push_back(alt);
      }
    }

    terrain.w = w;
    terrain.h = h;

    return terrain;
  }
  Structure_Instance::
  Structure_Instance(IStructure* s, Orient o) noexcept
    : structure_type(s), orientation(o) {}
  Structure_Instance::Structure_Instance(Structure_Instance const& s) noexcept
    : structure_type(s.structure_type), orientation(s.orientation) {}
}
