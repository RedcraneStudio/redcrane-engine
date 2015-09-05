/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#pragma once
#include "terrain.h"
namespace game { namespace gen
{
  struct Radial_Algorithm : public Terrain_Algorithm
  {
    void gen(Grid_Map&) noexcept override;

    Vec<int> origin;

    float radius;

    float amplitude;
    float frequency;

    // Multiplied by amplitude after each octave
    float persistence;

    // Multiplied by frequency after each octave
    float lacunarity;

    int octaves;

    Cell_Type type;
  };
} }
