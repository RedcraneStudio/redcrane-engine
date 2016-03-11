/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "sdl_helper.h"
#include "common/log.h"
namespace redc
{
  SDL_Init_Lock::SDL_Init_Lock(std::string title, Vec<int> res, bool fs,
                               bool vsync) noexcept
  {
    window = init_sdl(title, res, fs, vsync);
  }
  SDL_Init_Lock::~SDL_Init_Lock() noexcept
  {
    uninit_sdl(window);
  }

  SDL_Window* init_sdl(std::string title, Vec<int> res, bool fullscreen,
                       bool vsync) noexcept
  {
    // Initialize SDL

    if(SDL_Init(SDL_INIT_VIDEO))
    {
      log_e("Failed to init SDL");
      return nullptr;
    }

    SDL_version version;
    SDL_GetVersion(&version);

    log_i("Initialized SDL %.%.%", version.major, version.minor, version.patch);

    // Initialize window

    auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    if(fullscreen) flags |= SDL_WINDOW_FULLSCREEN;

    auto* window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED, res.x, res.y,
                                    flags);
    if(!window)
    {
      log_e("Failed to initialize SDL for resolution %x%", res.x, res.y);
      return nullptr;
    }

    // Initialize OpenGL context

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    if(vsync) SDL_GL_SetSwapInterval(1);
    else SDL_GL_SetSwapInterval(0);

    auto context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, context);

    gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress);

    int opengl_maj, opengl_min;
    glGetIntegerv(GL_MAJOR_VERSION, &opengl_maj);
    glGetIntegerv(GL_MINOR_VERSION, &opengl_min);
    log_i("OpenGL core profile %.%", opengl_maj, opengl_min);

    return window;
  }

  void uninit_sdl(SDL_Window* window) noexcept
  {
    SDL_GL_DeleteContext(SDL_GL_GetCurrentContext());
    SDL_DestroyWindow(window);
    SDL_Quit();
  }
}
