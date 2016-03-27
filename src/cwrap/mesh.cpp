/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file provides the implementation for the engine's C interface,
 * specifically tailored for LuaJIT's FFI facilities.
 *
 * Mesh stuff
 */

#include "redcrane.hpp"

#include "../use/mesh.h"
#include "../use/mesh_cache.h"

using namespace redc;

extern "C"
{
  // See mesh_pool.lua
  void *redc_load_mesh(void *engine, const char *str)
  {
    // TODO: Load many static objects into the same mesh.
    auto rce = (redc::Engine *) engine;

    auto res = gfx::load_mesh(*rce->driver,
                              *rce->mesh_cache,
                              {std::string{str}, false});

    // Allocate a mesh_chunk to store it
    auto chunk = new gfx::Mesh_Chunk;
    *chunk = copy_mesh_chunk_move_mesh(res.chunk);
    return chunk;
  }
  void redc_unload_mesh(void *mesh)
  {
    auto chunk = (gfx::Mesh_Chunk *) mesh;
    delete chunk;
  }
}
