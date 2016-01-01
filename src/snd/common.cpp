/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include <limits>
#include <algorithm>

#include "common.h"
#include "../common/log.h"

#include "pulse/simple.h"
#include "pulse/error.h"
namespace redc { namespace snd
{
  void initialize_pa() noexcept {}
  void terminate_pa() noexcept {}

  struct Stream_Impl
  {
    pa_simple* stream;

    PCM_Data* pcm_data;
    bool done;
  };

  Stream::Stream() noexcept : impl(new Stream_Impl())
  {
    pa_sample_spec ss;

    ss.rate = 44100;
    ss.channels = 2;
    ss.format = PA_SAMPLE_S16LE;

    int err = 0;
    impl->stream = pa_simple_new(NULL, "He came with the dust...",
                                 PA_STREAM_PLAYBACK,
                                 NULL, "Like Wind Blows Fire", &ss,
                                 NULL, NULL, &err);
    if(err)
    {
      log_e("... And was gone with the wind! %", pa_strerror(err));
    }
  }

  Stream::Stream(Stream&& s) noexcept : impl(s.impl)
  {
    s.impl = nullptr;
  }
  Stream& Stream::operator=(Stream&& s) noexcept
  {
    this->impl = s.impl;
    s.impl = nullptr;

    return *this;
  }

  Stream::~Stream() noexcept
  {
    if(impl->stream)
    {
      pa_simple_free(impl->stream);
    }

    delete impl;
  }

  void Stream::use_pcm(PCM_Data& data) noexcept
  {
    impl->pcm_data = &data;
  }
  void Stream::start() noexcept
  {
    if(!impl->pcm_data)
    {
      log_e("No PCM data!");
      return;
    }

    // Get the bytes
    int err = 0;

    pa_simple_write(impl->stream, &impl->pcm_data->samples[0],
                    impl->pcm_data->samples.size() * sizeof(Sample), &err);
    if(err) log_e("When writing: %", pa_strerror(err));

    err = 0;

    pa_simple_drain(impl->stream, NULL);
    if(err) log_e("When draining: %", pa_strerror(err));
  }
  void Stream::stop(bool) noexcept {}
} }
