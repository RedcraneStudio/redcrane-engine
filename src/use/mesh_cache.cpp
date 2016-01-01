/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "mesh_cache.h"
namespace redc { namespace gfx
{
  Mesh_Chunk Mesh_Cache::load_owned_chunk(std::string filename) noexcept
  {
    return load_mesh(*driver_, {filename}).chunk;
  }
} }

