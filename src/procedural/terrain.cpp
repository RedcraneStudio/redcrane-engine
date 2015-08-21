/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#include "terrain.h"

extern "C"
{
  #include "open-simplex-noise.h"
}
namespace game { namespace strat
{
  void gen_noise_heightmap(int64_t seed, Value_Map<float>& map) noexcept
  {
    osn_context* context;
    open_simplex_noise(seed, &context);

    for(int i = 0; i < map.extents.x * map.extents.y; ++i)
    {
      auto x = i % map.extents.x;
      auto y = i / map.extents.x;

      open_simplex_noise2(context, x, y);
    }

    open_simplex_noise_free(context);
  }
} }
