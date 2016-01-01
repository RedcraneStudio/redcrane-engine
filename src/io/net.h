/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <uv.h>
#include <stdexcept>
#include <string>
#include <cstdint>
#include "buffer.h"

namespace redc { namespace net
{
  // Exceptions
  struct Bind_Error
  {
    const std::string& bind_address;
  };

  struct UDP_Handle
  {
    uv_udp_t handle;
    buf_t* buf;
  };

  UDP_Handle* create_udp_handle(uv_loop_t*) noexcept;
  void delete_udp_handle(UDP_Handle*) noexcept;
  void init_udp_handle(UDP_Handle&, uv_loop_t*) noexcept;
  void uninit_udp_handle(UDP_Handle&) noexcept;

  struct Pipe
  {
    UDP_Handle in;
    UDP_Handle out;

    void* user_data;

    using action_cb = void(*)(Pipe*);
    action_cb read_cb;
    action_cb write_cb;
  };

  Pipe* create_pipe(uv_loop_t*, std::string const& bind_ip,
                    uint16_t const port);

  void delete_pipe(Pipe*) noexcept;

  void init_pipe(Pipe&, uv_loop_t*, std::string const& bind_ip,
                 uint16_t const port);

  void uninit_pipe(Pipe&) noexcept;

  void write_buffer(Pipe& p, sockaddr const* dest) noexcept;
} }
