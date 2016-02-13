/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "steamwrap.h"
#include "steam/steam_api.h"
namespace redc { namespace steam
{
  Scoped_Steam_Init::Scoped_Steam_Init()
  {
    SteamAPI_Init();
  }
  Scoped_Steam_Init::~Scoped_Steam_Init()
  {
    SteamAPI_Shutdown();
  }
} }
