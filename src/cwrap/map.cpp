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

#include "../gfx/find_string_index.hpp"
#include "../common/log.h"

using namespace redc;

extern "C"
{
  void redc_map_load(void* eng, const char* file)
  {
    auto rce = (Engine*) eng;
    REDC_ASSERT_HAS_SERVER(rce);

    rce->server->load_map(std::string{file});
  }
  Redc_Light_State redc_map_get_light_state(void *map_ptr, const char *light)
  {
    redc::Map* map = (redc::Map*) map_ptr;
    redc::gfx::Light_Ref ref =
      gfx::find_string_index(map->render->asset.light_names, light,
                             "Cannot find light %", light);

    Redc_Light_State state;
    state.on = map->render->asset.lights[ref].is_active;
    return state;
  }
  int redc_map_set_light_state(void *map_ptr, const char *light,
                               Redc_Light_State state)
  {
    redc::Map* map = (redc::Map*) map_ptr;
    redc::gfx::Light_Ref ref =
      gfx::find_string_index(map->render->asset.light_names, light,
                             "Cannot find light %", light);

    map->render->asset.lights[ref].is_active = state.on;
  }
}
