/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "water.h"
#include "../common/noise.h"
#include "terrain.h"
namespace redc { namespace strat
{
  void set_noise_heightmap(Heightmap& h, double time) noexcept
  {
    for(int i = 0; i < h.extents.y * h.extents.y; ++i)
    {
      int x = i % h.extents.x;
      int y = i / h.extents.x;

      h.vals[i] = perlin_noise(x / 10.0f, y / 10.0f, time) * 100;
    }
  }
} }
