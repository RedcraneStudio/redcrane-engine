/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#pragma once

extern "C"
{
  #include "open-simplex-noise.h"
}
#include "noise.h"
#include "vec.h"

namespace redc
{
  // T must have the following members:
  // - float amplitude;
  // - float frequency;
  // - float persistence;
  // - float lacunarity;
  // - int octaves;

  // Let this commemorate the moment Hazza rekt Navy at precisely (9-3-15,
  // 17:49:23)!   <----------------.------------------------^
  template <class P, class T>   // ^---------------------------.
  float gen_noise(Vec<P> pt, T const& rekt) noexcept // |
  {                                                   // ^-----^
    auto value = 0.0f;
    auto cur_amplitude = rekt.amplitude;
    auto cur_frequency = rekt.frequency;
    for(int octave_i = 0; octave_i < rekt.octaves; ++octave_i)
    {
      value += perlin_noise(pt.x + cur_frequency, pt.y + cur_frequency, 1.0);

      cur_amplitude *= rekt.persistence;
      cur_frequency *= rekt.lacunarity;
    }

    return value;
  }
  template <class P, class T>
  float gen_simplex_noise(osn_context* osn, Vec<P> pt, T const& rekt) noexcept
  {
    auto value = 0.0f;
    auto cur_amplitude = rekt.amplitude;
    auto cur_frequency = rekt.frequency;
    for (int octave_i = 0; octave_i < rekt.octaves; ++octave_i)
    {
      value += open_simplex_noise2(osn, pt.y * cur_frequency,
                                   pt.x * cur_frequency) * cur_amplitude;
      cur_amplitude *= rekt.persistence;
      cur_frequency *= rekt.lacunarity;
    }

    return value;
  }
}
