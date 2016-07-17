/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include <limits>
#include <algorithm>

#include "common.h"
#include "../common/log.h"

#include "SDL.h"
namespace redc { namespace snd
{
  struct Stream_Impl
  {
    SDL_AudioDeviceID device_id;

    PCM_Data* pcm_data;
    bool done;
  };

  Stream::Stream() : impl(std::make_unique<Stream_Impl>())
  {
    if(SDL_InitSubSystem(SDL_INIT_AUDIO))
    {
      log_e("Failed to initialize audio subsystem: %", SDL_GetError());
      impl.release();
      return;
    }
    else
    {
      const char* audio_driver = SDL_GetCurrentAudioDriver();
      if(audio_driver)
      {
        log_i("Using audio driver: %", audio_driver);
      }
    }

    SDL_AudioSpec in_format;
    in_format.freq = 44100;
    in_format.format = AUDIO_S16LSB;
    in_format.channels = 2;
    in_format.samples = 4096;
    in_format.callback = NULL;

    impl->device_id = SDL_OpenAudioDevice(NULL, 0, &in_format, NULL, 0);
    if(impl->device_id == 0)
    {
      log_e("Failed to initialize audio device: %", SDL_GetError());
      impl.release();
    }

    SDL_PauseAudioDevice(impl->device_id, 0);
  }

  Stream::~Stream()
  {
    if(impl)
    {
      SDL_CloseAudioDevice(impl->device_id);
    }
  }

  void Stream::use_pcm(PCM_Data& data)
  {
    if(!impl) return;
    impl->pcm_data = &data;
  }
  void Stream::start()
  {
    if(!impl || !impl->pcm_data)
    {
      log_e("No PCM data!");
      return;
    }

    // Queue all of the audio
    SDL_QueueAudio(impl->device_id, &impl->pcm_data->samples[0].left,
                   sizeof(Sample) * impl->pcm_data->samples.size());
  }
  void Stream::stop(bool) {}
} }
