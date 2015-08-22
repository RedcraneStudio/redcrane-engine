/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#pragma once
#include <cstdint>

#include "../common/value_map.h"
namespace game { namespace strat
{
  // The value map should have been allocated.
  void gen_noise_heightmap(int64_t seed, Value_Map<float>& map) noexcept;

  void write_png_heightmap(Value_Map<float> const& map,
                           std::string const&) noexcept;
} }
