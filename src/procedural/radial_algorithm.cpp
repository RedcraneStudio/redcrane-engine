/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#include "radial_algorithm.h"
#include "../common/volume.h"
namespace game { namespace strat
{
  // This function could be run alone.
  void Radial_Algorithm::gen(Grid_Map& map) noexcept
  {
    osn_context* osn = nullptr;
    auto noise_init = Noise_Raii{this->seed, &osn};

    // Figure out what area we actually *could* modify. I think technically it
    // could be big larger if we are dealing with multiple octaves, but
    // whatever.
    Volume<int> active_area;
    active_area.pos.x = std::ceil(origin.x - radius - amplitude);
    active_area.pos.y = std::ceil(origin.y - radius - amplitude);
    active_area.width = (radius + amplitude) * 2;
    active_area.height = (radius + amplitude) * 2;

    active_area = contain_inside_extents(active_area, map.extents);

    auto& aa = active_area;
    for(int i = aa.pos.y; i < aa.height + aa.pos.y; ++i)
    {
      for(int j = aa.pos.x; j < aa.width + aa.pos.x; ++j)
      {
        // Point from the continent origin to here.
        Vec<float> to_here = Vec<int>{j, i} - this->origin;
        auto to_here_dir = normalize(to_here);

        // Angle is not used or passed into the noise function because at 2pi
        // radians it goes from +pi to -pi causing a discontinuity in the land
        // mass.

        // Get a small delta from the angle. We will use this to modify the
        auto delta = 0.0f;
        auto cur_amplitude = this->amplitude;
        auto cur_frequency = this->frequency;
        for(int octave_i = 0; octave_i < this->octaves; ++octave_i)
        {
          delta += open_simplex_noise2(osn, to_here_dir.y * cur_frequency,
                                       to_here_dir.x * cur_frequency) *
                   cur_amplitude;
          cur_amplitude *= this->persistence;
          cur_frequency *= this->lacunarity;
        }

        auto modified_radius = radius + delta;

        // This tile is land.
        if(length(to_here) < modified_radius)
        {
          map.values[i * map.extents.x + j].type = this->type;
        }
      }
    }
  }
} }
