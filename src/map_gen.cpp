/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#include <random>
#include <cstdlib>
#include <iostream>

#include "common/log.h"
#include "procedural/terrain.h"

#include "luaint/common.h"

int main(int argc, char** argv)
{
  using namespace game;

  Scoped_Log_Init log_init{};

  strat::Grid_Map map;
  map.allocate({2048, 2048});

  std::random_device rand_dev;
  std::mt19937 prng{rand_dev()};

  strat::Terrain_Params terrain_params;
  terrain_params.seed = prng();
  terrain_params.origin = map.extents / 2;
  terrain_params.size = 1448.0f;
  terrain_params.algorithm = strat::Landmass_Algorithm::Radial;
  terrain_params.radial = strat::Radial_Landmass{300.0f, 1.0f, .5f, 2.0f, 8};

  if(argc >= 2)
  {
    std::string dir_name = argv[1];

    auto L = luaint::init_lua();
    luaint::load_mod(*L, dir_name);
    luaint::uninit_lua(L);
  }

  strat::terrain_v1_map(map, terrain_params);
  strat::write_png_heightmap(map, "terrain.png");

  return 0;
}
