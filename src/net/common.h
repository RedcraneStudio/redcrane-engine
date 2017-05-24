/*
 * Copyright (C) 2017 Luke San Antonio
 * All rights reserved.
 */

#ifndef REDC_ENGINE_COMMON_H
#define REDC_ENGINE_COMMON_H
#include <cstdint>
#include <msgpack.hpp>
#include "../input/input.h"
namespace redc
{
  constexpr static uint16_t PROTOCOL_VERSION = 1;

  // Hahahaha fuck it
  using okay_t = bool;

  // First the version information
  using version_t = uint16_t;

  struct Version_Info
  {
    version_t protocol_version;
    version_t client_version;

    MSGPACK_DEFINE(protocol_version, client_version);
  };

  struct Version_Okay
  {
    okay_t protocol;
    okay_t client;

    MSGPACK_DEFINE(protocol, client);
  };

  // Yah I'm going to have to say a maximum of 65,535 teams, sorry.
  // I know... I know.
  using team_id = uint16_t;

  using player_id = uint16_t;

  struct Input_Update
  {
    uint8_t index;
    Input input;

    MSGPACK_DEFINE(index, input);
  };
}
#endif //RED_CRANE_ENGINE_COMMON_H
