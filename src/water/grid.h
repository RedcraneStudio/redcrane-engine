/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../common/vec.h"
#include "../common/value_map.h"
#include "../gfx/mesh_data.h"
#include "../gfx/texture.h"

extern "C"
{
  #include "opensimplex/open-simplex-noise.h"
}

namespace game { namespace water
{
  // Ordered_Mesh_Data gen_grid(Vec<int> extents) noexcept;

  struct Water
  {
    Water(int64_t seed, Vec<int> grid_extents) noexcept;
    ~Water() noexcept;

    osn_context* osn;

    Vec<int> extents;
    Value_Map<float> heightmap;
    Value_Map<glm::vec3> normalmap;
  };

  struct Noise_Gen_Params
  {
    float amplitude;
    float frequency;
    float persistence;
    float lacunarity;

    int octaves;
  };

  template <class T>
  void gen_heightmap(Water& water, double time, T const& params) noexcept
  {
    // Sum up some noise.
    for(int i = 0; i < water.heightmap.extents.x * water.heightmap.extents.y;
        ++i)
    {
      auto x = i % water.heightmap.extents.x;
      auto y = i / water.heightmap.extents.x;

      auto value = 0.0f;
      auto cur_amplitude = params.amplitude;
      auto cur_frequency = params.frequency;
      for(int octave_i = 0; octave_i < params.octaves; ++octave_i)
      {
        value += open_simplex_noise3(water.osn, x * cur_frequency,
                                     y * cur_frequency,
                                     time) * cur_amplitude;
        cur_amplitude *= params.persistence;
        cur_frequency *= params.lacunarity;
      }

      auto value_clamped = (value + 1.0f) / 2.0f;
      value_clamped = std::max(0.0f, std::min(1.0f, value_clamped));
      water.heightmap.at({x, y}) = value_clamped;
    }
  }
  void gen_normalmap(Water& water) noexcept;

  void blit_normalmap(Texture& tex, Water& water) noexcept;
  void blit_heightmap(Texture& tex, Water& water) noexcept;

  void write_heightmap_png(std::string fn, Water& water) noexcept;
  void write_normalmap_png(std::string fn, Water& water) noexcept;
} }
