/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <glm/glm.hpp>
namespace redc
{
  double perlin_noise(double x, double y, double z) noexcept;
  float cnoise(glm::vec2 P);

  template <class T>
  float fbm(glm::vec2 P, T const& params)
  {
    float sum = 0.0;

    float cur_freq = params.frequency;
    float cur_ampl = params.amplitude;

    int i;

    for(i = 0; i < params.octaves; i+=1)
    {
      sum += cur_ampl * cnoise(P * cur_freq);
      cur_freq *= params.persistence;
      cur_ampl *= params.lacunarity;
    }
    return sum;
  }
}
