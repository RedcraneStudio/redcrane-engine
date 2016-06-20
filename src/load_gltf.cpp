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

  redc::Rendering_State cur_rendering_state;

  redc::gfx::Camera cam = redc::gfx::make_fps_camera({1000,1000});

  cam.fp.pos.z = 10.0f;

  glEnable(GL_DEPTH_TEST);

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  SDL_SetRelativeMouseMode(SDL_TRUE);

  bool running = true;
  while(running)
  {
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
      switch(event.type)
      {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_KEYDOWN:
        if(event.key.keysym.scancode == SDL_SCANCODE_W) cam.fp.pos.y += .1f;
        if(event.key.keysym.scancode == SDL_SCANCODE_S) cam.fp.pos.y -= .1f;
        if(event.key.keysym.scancode == SDL_SCANCODE_A) cam.fp.pos.x -= .1f;
        if(event.key.keysym.scancode == SDL_SCANCODE_D) cam.fp.pos.x += .1f;
        if(event.key.keysym.scancode == SDL_SCANCODE_Q) cam.fp.pos.z -= .1f;
        if(event.key.keysym.scancode == SDL_SCANCODE_E) cam.fp.pos.z += .1f;
        break;
      case SDL_MOUSEMOTION:
        cam.fp.yaw += .001f * event.motion.xrel;
        cam.fp.pitch += .001f * event.motion.yrel;
      default:
        break;
      }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render_asset(asset, cam, cur_rendering_state);

    SDL_GL_SwapWindow(sdl_init.window);
  }

  return EXIT_SUCCESS;
}
