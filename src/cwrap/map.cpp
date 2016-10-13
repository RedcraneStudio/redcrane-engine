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

  size_t redc_map_get_num_lights(void *map_ptr)
  {
    redc::Map* map = (redc::Map*) map_ptr;
    return map->render->asset.light_names.size();
  }
  const char *redc_map_get_light_name(void *map_ptr, size_t index)
  {
    redc::Map* map = (redc::Map*) map_ptr;
    return map->render->asset.light_names[index].c_str();
  }

  Redc_Light_State redc_map_get_light_i_state(void *map_ptr, size_t ref)
  {
    redc::Map* map = (redc::Map*) map_ptr;

    Redc_Light_State state;
    state.power = map->render->asset.lights[ref].intensity;
    return state;
  }
  Redc_Light_State redc_map_get_light_state(void *map_ptr, const char *light)
  {
    redc::Map* map = (redc::Map*) map_ptr;
    redc::gfx::Light_Ref ref =
      gfx::find_string_index(map->render->asset.light_names, light,
                             "Cannot find light %", light);
    return redc_map_get_light_i_state(map_ptr, ref);
  }
  int redc_map_set_light_i_state(void *map_ptr, size_t ref,
                                Redc_Light_State state)
  {
    redc::Map* map = (redc::Map*) map_ptr;
    if(ref >= map->render->asset.lights.size()) return REDC_FALSE;

    map->render->asset.lights[ref].intensity = state.power;
    return REDC_TRUE;
  }
  int redc_map_set_light_state(void *map_ptr, const char *light,
                               Redc_Light_State state)
  {
    redc::Map* map = (redc::Map*) map_ptr;

    // TODO: Instead of crashing, return false, for failure.
    redc::gfx::Light_Ref ref =
      gfx::find_string_index(map->render->asset.light_names, light,
                             "Cannot find light %", light);

    return redc_map_set_light_i_state(map_ptr, ref, state);
  }
}
