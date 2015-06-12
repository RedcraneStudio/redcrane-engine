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

  if(argc < 2)
  {
    // Fuck you, user
    log_e("Pass in a file!");
    return 1;
  }

  snd::initialize_pa();

  {
    auto stream = snd::Stream{};

    auto flac = snd::load_flac(argv[1]);

#if 0
    auto f = fopen("data.pcm", "wb");
    for(auto i : flac.samples)
    {
      fwrite(&i.left, sizeof(i), 1, f);
    }
#endif

    stream.use_pcm(flac);

    stream.start();
  }

  snd::terminate_pa();
}
