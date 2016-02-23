/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 *
 * The engine's C interface for the engine to use
 */
#include <cstdint>

extern "C"
{
#include "redcrane_decl.h"
}

enum class Engine_State
{
  None, Client
};

struct Redc_Engine
{
  Engine_State state;

  boost::program_options::variables_map config;
};
