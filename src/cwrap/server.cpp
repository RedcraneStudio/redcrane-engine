/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file provides the implementation for the engine's C interface,
 * specifically tailored for LuaJIT's FFI facilities.
 *
 * Server stuff
 */

#include "redcrane.hpp"

using namespace redc;

void redc_server_req_player(void *eng)
{
  auto rce = (Engine*) eng;
  REDC_ASSERT_HAS_SERVER(rce);

  rce->server->req_player();
}
