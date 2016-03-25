/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 *
 * The engine's C interface for the engine to use
 */
#include <cstdint>
#include <memory>

#include "minilua.h"

#include "common/id_map.hpp"

#include "use/mesh_cache.h"

#include "gfx/idriver.h"
#include "gfx/camera.h"

#include "SDL.h"

namespace redc
{
  struct Engine
  {
    std::unique_ptr<gfx::IDriver> driver;
    std::unique_ptr<gfx::Mesh_Cache> mesh_cache;
    SDL_Window* window;

    bool running = true;

    // TODO: Maybe keep track of every scene so lua doesn't have to deal with it
    // We should reserve some amount of memory for each scene so that we can
    // pass around pointers and no they won't suddenly become invalid.
  };
  struct Scene
  {
    // This is an unordered map that also keeps an active camera available to
    // us.
    Engine* engine;
    Active_Map<gfx::Camera> cams;
  };
}
