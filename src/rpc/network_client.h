/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../common/maybe_owned.hpp"
#include "../io/net_io.h"
#include "msgpack_interface.h"
namespace redc
{
  struct Network_Client
  {
    // This is a little bit redundant but retains type information.
    Maybe_Owned<Net_IO> io;
    rpc::Msgpack_Interface plugin;
  };
}
