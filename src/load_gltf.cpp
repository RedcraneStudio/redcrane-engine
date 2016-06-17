/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#include <chrono>
#include "sdl_helper.h"
#include "gfx/scene.h"
#include "assets/minigltf.h"
#include "common/log.h"
int main(int argc, char** argv)
{
  redc::Scoped_Log_Init log_raii{};
  if(argc < 2)
  {
    redc::log_e("usage: % <filename.gltf>", argv[0]);
    return EXIT_FAILURE;
  }

  auto sdl_init = init_sdl("glTF viewer", redc::Vec<int>{1000, 1000}, false,
                           false);

  auto before = std::chrono::high_resolution_clock::now();
  auto desc = redc::load_gltf_file(argv[1]);
  auto asset = redc::load_asset(desc.value());
  auto after = std::chrono::high_resolution_clock::now();

  auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
  redc::log_i("Took %s to load", dt.count() / 1000.0);

  return EXIT_SUCCESS;
}
