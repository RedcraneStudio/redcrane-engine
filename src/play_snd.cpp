/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include <thread>

#include "common/log.h"

#include "snd/common.h"
#include "snd/pcm_data.h"
#include "snd/flac.h"

#include "portaudio.h"

int main(int argc, char** argv)
{
  using namespace game;

  Scoped_Log_Init log_init{};

  int device = 0;


  if(argc < 4)
  {
    // Fuck you, user
    log_e("Pass in file, time, and device, faggot!");


    snd::initialize_pa();

    // Print available devices.
    int device_count = Pa_GetDeviceCount();
    for(int i = 0; i < device_count; ++i)
    {
      auto device_info = Pa_GetDeviceInfo(i);
      log_e("Device %) %", i + 1, device_info->name);
    }

    snd::terminate_pa();
    return 1;
  }
  else
  {
    device = std::atoi(argv[3]);
    if(device == 0)
    {
      log_e("Bad device number!");
      return 1;
    }
    device -= 1;
  }

  snd::initialize_pa();

  {
    auto stream = snd::Stream{device};

    auto flac = snd::load_flac(argv[1]);
    stream.use_pcm(flac);

    stream.start();
    std::this_thread::sleep_for(std::chrono::seconds(std::atoi(argv[2])));
    stream.stop(true);
  }

  snd::terminate_pa();
}
