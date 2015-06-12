/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <cstdint>
#include <vector>
namespace game { namespace snd
{
  struct Sample
  {
    int16_t left;
    int16_t right;
  };
  struct PCM_Data
  {
    int bits_per_sample;

    std::vector<Sample> samples;
    bool error;
  };
} }
