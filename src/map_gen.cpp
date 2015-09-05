/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#include <random>
#include <cstdlib>
#include <iostream>

#include "common/log.h"
#include "gen/terrain.h"
#include "gen/radial_algorithm.h"
#include "gen/tree_gen.h"

#include "luaint/common.h"

int main(int argc, char** argv)
{
  using namespace game;

  Scoped_Log_Init log_init{};

  gen::Grid_Map map;
  map.allocate({2048, 2048});

  std::random_device rand_dev;
  std::mt19937 prng{rand_dev()};

  gen::Terrain_Params terrain_params;
  terrain_params.seed = prng();

  auto landmass_gen = std::make_unique<gen::Radial_Algorithm>();
  landmass_gen->origin = map.extents / 2;
  landmass_gen->radius = 724.0f;
  landmass_gen->amplitude = 300.0f;
  landmass_gen->frequency = 1.0f;
  landmass_gen->persistence = .5f;
  landmass_gen->lacunarity = 2.0f;
  landmass_gen->octaves = 8;
  landmass_gen->type = gen::Cell_Type::Land;

  terrain_params.landmass_gen = std::move(landmass_gen);

  auto natural_gen = std::make_unique<gen::Tree_Gen_Algorithm>();
  natural_gen->amplitude = 1.0f;
  natural_gen->frequency = 0.01f;
  natural_gen->persistence = .5f;
  natural_gen->lacunarity = 2.0f;
  natural_gen->octaves = 3;
  natural_gen->tree_altitude = 0.4f;

  terrain_params.natural_gen = std::move(natural_gen);

  if(argc >= 2)
  {
    std::string dir_name = argv[1];

    auto L = luaint::init_lua();
    log_w("Amount of mod types %", luaint::num_mod_types(*L));

    luaint::load_mod(*L, dir_name);
    log_w("Number of registered mods %", luaint::registered_mods(*L));

    luaint::uninit_lua(L);
  }

  gen::terrain_v1_map(map, terrain_params);
  gen::write_png_heightmap(map, "terrain.png");

  return 0;
}
