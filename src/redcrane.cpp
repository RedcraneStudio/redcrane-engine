/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file provides the implementation for the engine's C interface,
 * specifically tailored for LuaJIT's FFI facilities.
 */

#include <cstdint>

#include "common/log.h"
#include "use/mesh.h"

#include "redcrane.hpp"

using namespace redc;

extern "C"
{
  void* redc_load_mesh(void* engine, const char* str)
  {
    auto rce = (redc::Engine*) engine;

    auto res = gfx::load_mesh(rce->driver, {std::string{str}, false});

    // Allocate a mesh_chunk to store it
    auto chunk = (gfx::Mesh_Chunk*) malloc(sizeof(gfx::Mesh_Chunk));
    *chunk = copy_mesh_chunk_move_mesh(res.chunk);
    return chunk;
  }
  void redc_draw_mesh(void* engine, void* mesh)
  {
    gfx::Mesh_Chunk* chunk = (gfx::Mesh_Chunk*) mesh;
    gfx::render_chunk(*chunk);
  }

  void redc_swap_window(void* engine)
  {
    auto rce = (redc::Engine*) engine;
    SDL_GL_SwapWindow(rce->window);
  }
}
