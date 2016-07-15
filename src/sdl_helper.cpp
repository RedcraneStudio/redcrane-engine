/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "sdl_helper.h"
#include "common/log.h"
namespace redc
{
  SDL_Init_Lock::SDL_Init_Lock(SDL_Init_Lock&& l) : window(l.window),
                                                    gl_context(l.gl_context)
  {
    l.window = nullptr;
    l.gl_context = nullptr;
  }

  SDL_Init_Lock& SDL_Init_Lock::operator=(SDL_Init_Lock&& l)
  {
    window = l.window;
    gl_context = l.gl_context;

    l.window = nullptr;
    l.gl_context = nullptr;

    return *this;
  }

  SDL_Init_Lock::~SDL_Init_Lock()
  {
    // If we don't have both for any reason, don't bother deallocating either.
    if(window && gl_context)
    {
      // That way, if we accidentally partially move we won't do the aggressive
      // SDL_Quit call.

      SDL_GL_DeleteContext(gl_context);
      SDL_DestroyWindow(window);
      SDL_Quit();
    }
  }

  SDL_Init_Lock init_sdl(std::string title, Vec<int> res, bool fullscreen,
                         bool vsync)
  {
    // Initialize SDL

    if(SDL_Init(SDL_INIT_VIDEO))
    {
      log_e("Failed to init SDL");
      return {};
    }

    SDL_version version;
    SDL_GetVersion(&version);

    log_i("Initialized SDL %.%.%", version.major, version.minor, version.patch);

    // Initialize window

    int flags = SDL_WINDOW_OPENGL;
    if(fullscreen) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    SDL_Init_Lock ret;

    ret.window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED, res.x, res.y,
                                  flags);
    if(!ret.window)
    {
      SDL_Quit();
      log_e("Failed to initialize SDL for resolution %x%", res.x, res.y);
      return ret;
    }

    // Initialize OpenGL context

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

    ret.gl_context = SDL_GL_CreateContext(ret.window);
    SDL_GL_MakeCurrent(ret.window, ret.gl_context);

    // Set vsync setting
    int sync_interval = 0;
    if(vsync) sync_interval = 1;
    if(SDL_GL_SetSwapInterval(sync_interval) == -1)
    {
      // Shit, failed to set that
      log_w("Failed to set swap interval: %", SDL_GetError());
    }

    gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress);

    int opengl_maj, opengl_min;
    glGetIntegerv(GL_MAJOR_VERSION, &opengl_maj);
    glGetIntegerv(GL_MINOR_VERSION, &opengl_min);
    log_i("OpenGL core profile %.%", opengl_maj, opengl_min);

    return ret;
  }
}
