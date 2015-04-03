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
    return terrain;
  }
}
