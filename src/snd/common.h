/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include "pcm_data.h"
namespace game { namespace snd
{
  void initialize_pa() noexcept;
  void terminate_pa() noexcept;

  struct Stream_Impl;
  struct Stream
  {
    Stream() noexcept;

    Stream(Stream const& s) noexcept = delete;
    Stream& operator=(Stream const& s) noexcept = delete;

    Stream(Stream&& s) noexcept;
    Stream& operator=(Stream&& s) noexcept;

    ~Stream() noexcept;

    Stream_Impl* impl;

    void use_pcm(PCM_Data&) noexcept;
    void start() noexcept;
    void stop(bool force = false) noexcept;
  };
} }
