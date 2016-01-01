/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#pragma once
#include "terrain.h"
namespace redc { namespace gen
{
  struct Lake_Radial_Algorithm : public Terrain_Algorithm
  {
    void gen(Grid_Map&) noexcept override;

    // Maximum amount of lakes
    int max_lakes;

    // Probably of lakes spawning out of max_lakes.
    float lake_probability;

    float min_radius;
    float max_radius;

    float amplitude;
    float frequency;
    float persistence;
    float lacunarity;

    int octaves;
  };
} }
