/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include <limits>

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

    PCM_Data* pcm_data;
    unsigned int where;
    bool done;
  };

  int snd_callback(const void* input, void* output, unsigned long frame_count,
                   const PaStreamCallbackTimeInfo* time_info,
                   PaStreamCallbackFlags status_flag,
                   void* user_data) noexcept
  {
    auto stream_impl = reinterpret_cast<Stream_Impl*>(user_data);

    auto* out = reinterpret_cast<float*>(output);

    auto& where = stream_impl->where;
    for(unsigned int i = 0; i < frame_count; ++i, ++where)
    {
      if(where >= stream_impl->pcm_data->samples.size())
      {
        // We are done here.
        stream_impl->done = true;
        log_i("Out of data");
        break;
      }

      out[i] = (float) stream_impl->pcm_data->samples[where].left /
               std::numeric_limits<uint32_t>::max();
      out[i+1] = (float) stream_impl->pcm_data->samples[where].right /
                 std::numeric_limits<uint32_t>::max();
    }

    if(stream_impl->done)
      return paComplete;
    else
      return paContinue;
  }

  Stream::Stream() noexcept : impl(new Stream_Impl())
  {
    // We give the user data in terms of the impl so that moving a stream won't
    // make the portaudio callback erroneously fail.
    auto err = Pa_OpenDefaultStream(&impl->stream, 0, 2, paFloat32,
                                    44100, paFramesPerBufferUnspecified,
                                    snd_callback, impl);
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

  void Stream::use_pcm(PCM_Data& data) noexcept
  {
    impl->done = false;
    impl->pcm_data = &data;
  }
} }
