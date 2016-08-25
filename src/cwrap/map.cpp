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
}
