/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#pragma once
#include <string>
#include <cstdint>

#include "../common/value_map.h"
namespace game { namespace strat
{
  struct Terrain_Params
  {
    int64_t seed;
    int octaves;
    float base_frequency;
    float base_amplitude = 1.0f;
  };

  // The value map should have been allocated.
  void gen_noise_heightmap(Value_Map<float>& map,
                           Terrain_Params params) noexcept;

  void write_png_heightmap(Value_Map<float> const& map,
                           std::string const&) noexcept;
} }
