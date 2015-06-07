/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "common.h"
#include "portaudio.h"
#include "../common/log.h"
namespace game { namespace snd
{
  void initialize_pa() noexcept
  {
    auto err = Pa_Initialize();
    if(err != paNoError)
    {
      log_e("Failed to initialize portaudio. %", Pa_GetErrorText(err));
      return;
    }
  }
  void terminate_pa() noexcept
  {
    auto err = Pa_Terminate();
    if(err != paNoError)
    {
      log_e("Failed to terminate portaudio. %", Pa_GetErrorText(err));
      return;
    }
  }

  struct Stream_Impl
  {
    PaStream* stream;
  };

  int snd_callback(const void* input, void* output, unsigned long frame_count,
                   const PaStreamCallbackTimeInfo* time_info,
                   PaStreamCallbackFlags status_flag,
                   void* user_data) noexcept
  {
    return paComplete;
  }

  Stream::Stream() noexcept : impl(new Stream_Impl())
  {
    auto err = Pa_OpenDefaultStream(&impl->stream, 0, 2, paFloat32,
                                    44100, paFramesPerBufferUnspecified,
                                    snd_callback, nullptr);
    if(err != paNoError)
    {
      log_e("Failed to initialize portaudio stream. %", Pa_GetErrorText(err));
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
      auto err = Pa_CloseStream(impl->stream);
      if(err != paNoError)
      {
        log_e("Failed to close portaudio stream. %", Pa_GetErrorText(err));
      }
    }

    delete impl;
  }
} }
