/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <queue>
#include <msgpack.hpp>
#include "req.h"
#include "../common/maybe_owned.hpp"
#include "../io/external_io.h"
namespace redc { namespace rpc
{
  // Msgpack => Request
  // Request => Msgpack
  struct Msgpack_Interface
  {
    Msgpack_Interface(Maybe_Owned<External_IO> io) noexcept;
    ~Msgpack_Interface() noexcept = default;

    Msgpack_Interface(Msgpack_Interface&&) = default;
    Msgpack_Interface(Msgpack_Interface const&) noexcept = delete;

    Msgpack_Interface& operator=(Msgpack_Interface&&) = default;
    Msgpack_Interface& operator=(Msgpack_Interface const&) noexcept = delete;

    bool poll_request(Request& req);
    void post_request(Request const& res, bool rely = true) noexcept;

    inline External_IO& io() noexcept { return *this->io_; }
    inline External_IO const& io() const noexcept { return *this->io_; }
  private:
    Maybe_Owned<External_IO> io_;
    msgpack::unpacker unpacker_;
    std::queue<msgpack::object_handle> raw_reqs_;
  };
} }
