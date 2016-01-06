/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <cstdint>
#include <enet/enet.h>
#include "external_io.h"
#include "../common/result.h"
#include "../common/null_t.h"
namespace redc { namespace net
{
  enum class Error
  {
    ENet_Init_Failed,
    Host_Init_Failed,
    Connect_Failed
  };

  // RAII wrapper for ENetHost.
  struct Host
  {
    explicit Host(ENetHost* host) noexcept;
    Host(Host&& oh) noexcept;
    Host& operator=(Host&& oh) noexcept;
    ~Host() noexcept;

    void close() noexcept;

    ENetHost* host;
  };
} };
