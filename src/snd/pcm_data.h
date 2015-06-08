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
    uint32_t left;
    uint32_t right;
  };

  struct PCM_Data
  {
    std::vector<Sample> samples;
    bool error;
  };
} }
