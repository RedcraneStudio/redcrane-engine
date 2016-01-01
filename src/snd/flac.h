/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include "pcm_data.h"
namespace redc { namespace snd
{
  PCM_Data load_flac(std::string file) noexcept;
} }
