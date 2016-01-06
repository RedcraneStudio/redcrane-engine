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

  Result<Host, Error> make_server_host(uint16_t port,
                                       std::size_t max_peers) noexcept;
  Result<Host, Error> make_client_host() noexcept;

  struct Net_IO : public External_IO
  {
    Net_IO(Host&& host, ENetPeer* peer, read_cb r_cb = nullptr,
           read_cb e_cb = nullptr) noexcept;

    Net_IO(Net_IO&& io) noexcept;
    Net_IO& operator=(Net_IO&& io) noexcept;

    ~Net_IO() noexcept {}

    void set_reliable(bool rely) noexcept;
    void write(buf_t const& buf) noexcept override;
    void step() noexcept override;
  private:
    Host host_;
    ENetPeer* peer_;
    bool send_reliable_ = true;
  };

  ENetPeer* wait_for_connection(Host& host, uint32_t time) noexcept;
} };
