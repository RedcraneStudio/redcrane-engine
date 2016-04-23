/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file provides the implementation for the engine's C interface,
 * specifically tailored for LuaJIT's FFI facilities.
 *
 * Map stuff
 */

#include "redcrane.hpp"

#include "../gfx/support/load_wavefront.h"
#include "../gfx/support/mesh_conversion.h"

#include "../common/log.h"

using namespace redc;

extern "C"
{
  void* redc_map_load(void* eng, const char* file)
  {
    log_i("Loading map: '%'", file);

    // Right now this is a filename that points to a model
    auto rce = (Engine*) eng;

    // Load the map
    auto mesh_data = rce->mesh_cache->load(file);

    // Make a new map (peer)

    // Be careful about pointing to this memory, it is allocated on the heap
    // so we're perfectly fine to assume it will remain exactly what it is for
    // its lifetime, but if things change majorly make sure we don't start
    // pointing to things and then moving that data around. Check out the
    // implementation of Map::Map().
    auto map = new Peer_Ptr<Map>(new Map(std::move(mesh_data)));

    // Give the engine a peer.
    rce->maps.push_back(map->peer());

    // Do we have an active map?
    if(!rce->active_map.get())
    {
      rce->active_map = map->lock();
    }

    return map;
  }
}
