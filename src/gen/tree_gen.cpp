/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#include "../common/gen_noise.h"
#include "tree_gen.h"
namespace redc { namespace gen
{
  void Tree_Gen_Algorithm::gen(Grid_Map& map) noexcept
  {
    osn_context* osn = nullptr;
    auto noise_init = Noise_Raii{this->seed, &osn};

    for(int i = 0; i < map.extents.y; ++i)
    {
      for(int j = 0; j < map.extents.x; ++j)
      {
        Vec<int> pt{j,i};
        if(map.at(pt).type == Cell_Type::Land)
        {
          auto value = gen_simplex_noise(osn, pt, *this);
          if(value >= tree_altitude)
          {
            map.at(pt).contents = this->contents;
          }
        }
      }
    }
  }
} }
