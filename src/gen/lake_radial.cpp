/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#include "lake_radial.h"
#include "radial_algorithm.h"
#include <random>

namespace game { namespace gen
{
  void Lake_Radial_Algorithm::gen(Grid_Map& map) noexcept
  {
    // Generate random points until we find one on land
    std::uniform_int_distribution<int> width_dist(0, map.extents.x);
    std::uniform_int_distribution<int> height_dist(0, map.extents.y);

    std::mt19937 prng{this->seed};

    for(int lake_i = 0; lake_i < this->max_lakes; ++lake_i)
    {
      // Should we make this lake
      std::uniform_real_distribution<float> lake_prob_dist(0, 1.0f);
      auto val = lake_prob_dist(prng);

      // Continue to the next lake, this one is a no go.
      if(val > this->lake_probability) continue;

      // Generate a new lake point
      Vec<int> pt;
      do
      {
        pt = Vec<int>{width_dist(prng), height_dist(prng)};
      } while(map.at(pt).type != Cell_Type::Land);

      // Populate the settings of a new radial terrain algorithm.
      Radial_Algorithm radial_lake;
      radial_lake.origin = pt;
      radial_lake.seed = (int64_t) prng();

      std::uniform_real_distribution<float> radius_dist(this->min_radius,
                                                        this->max_radius);
      radial_lake.radius = radius_dist(prng);
      radial_lake.amplitude = this->amplitude;
      radial_lake.frequency = this->frequency;
      radial_lake.persistence = this->persistence;
      radial_lake.lacunarity = this->lacunarity;
      radial_lake.octaves = this->octaves;
      radial_lake.type = Cell_Type::Water;

      radial_lake.gen(map);
    }
  }
} }
