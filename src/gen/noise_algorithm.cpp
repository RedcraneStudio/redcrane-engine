/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#include "noise_algorithm.h"
#include "../common/log.h"
namespace game { namespace gen
{
  void Noise_Algorithm::gen(Grid_Map& map) noexcept
  {
    osn_context* osn = nullptr;
    auto noise_init = Noise_Raii{this->seed, &osn};

    auto use_area = contain_inside_extents(area, map.extents);
    for(int i = use_area.pos.y; i < use_area.pos.y + use_area.height; ++i)
    {
      for(int j = use_area.pos.x; j < use_area.pos.x + use_area.width; ++j)
      {
        float value = gen_noise(osn, Vec<int>{j, i}, *this);

        // Find our distance from the origin point.

        // Current position - Center
        auto origin = this->area.pos;
        origin.x += this->area.width / 2;
        origin.y += this->area.height / 2;

        auto distance = length(Vec<int>{j,i} - origin);

        auto value_modifier = 0.0f;
        if(distance > preferred_radius)
        {
          auto distance_away = distance - this->preferred_radius;
          value_modifier = distance_away * this->center_gradient_multiplier;
          value_modifier = std::max(0.0f, std::min(1.0f, value_modifier));
        }

        value -= value_modifier;
        if(value >= this->altitude)
        {
          map.at({j,i}).type = this->type;
        }
      }
    }
  }
} }
