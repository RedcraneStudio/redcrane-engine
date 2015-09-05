/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#pragma once
#include "terrain.h"
#include "../common/volume.h"
namespace game { namespace gen
{
  struct Noise_Algorithm : public Terrain_Algorithm
  {
    void gen(Grid_Map&) noexcept override;

    Volume<int> area;

    // Should be in the range [-1,1] and probably a little smaller in practice.
    // If the final sum of all octaves at a single cell point results in a
    // value greater than or equal to altitude, that cell is changed to the
    // given type.
    float altitude;

    // The starting value for their respective functions. Later modified by
    // persistence and lacunarity, respectively. Mind that setting amplitude
    // too high will result in very unsmooth transitions (I think).
    float amplitude;
    float frequency;

    // Multiplied by amplitude after each octave
    float persistence;

    // Multiplied by frequency after each octave
    float lacunarity;

    int octaves;

    float preferred_radius;

    // The amount per distance unit (from origin) to subtract from the noise
    // result
    float center_gradient_multiplier;

    Cell_Type type;
  };
} }
