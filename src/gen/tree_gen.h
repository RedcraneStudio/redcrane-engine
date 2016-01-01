/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#pragma once
#include "terrain.h"
namespace redc { namespace gen
{
  // Use open-simplex noise to generate a sort of forest biome on top of the
  // land. This forest area will later on be populated with trees, either
  // sparsely or densly, I'm not sure how just yet.
  struct Tree_Gen_Algorithm : public Terrain_Algorithm
  {
    void gen(Grid_Map&) noexcept override;

    float amplitude;
    float frequency;
    float persistence;
    float lacunarity;
    int octaves;

    // The value at which the noise will result in placing trees down.
    float tree_altitude;

    Cell_Contents contents = Cell_Contents::Tree;
  };
} }
