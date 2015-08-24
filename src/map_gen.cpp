/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#include <random>
#include <cstdlib>
#include <iostream>

#include "common/log.h"
#include "procedural/terrain.h"

int main(int argc, char** argv)
{
  int64_t seed = 0;
  if(argc >= 2)
  {
    seed = std::stoi(argv[1]);
  }
  else
  {
    std::random_device rand_dev;
    std::mt19937 prng{rand_dev()};
    seed = prng();
  }

  using namespace game;

  Scoped_Log_Init log_init{};

  strat::Grid_Map map;
  map.allocate({2048, 2048});

  strat::Terrain_Params terrain_params;
  terrain_params.seed = seed;
  terrain_params.origin = map.extents / 2;
  terrain_params.size = 1448.0f;
  terrain_params.algorithm = strat::Landmass_Algorithm::Radial;
  terrain_params.radial = strat::Radial_Landmass{300.0f, 1.0f, .5f, 2.0f, 8};

  strat::terrain_v1_map(map, terrain_params);
  strat::write_png_heightmap(map, "terrain.png");

  return 0;
}
