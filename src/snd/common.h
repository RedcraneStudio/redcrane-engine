/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include "pcm_data.h"
namespace redc { namespace snd
{
  struct Stream_Impl;
  struct Stream
  {
    Stream();
    ~Stream();

    std::unique_ptr<Stream_Impl> impl;

    void use_pcm(PCM_Data&);
    void start();
    void stop(bool force = false);
  };
} }
