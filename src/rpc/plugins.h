/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <queue>
#include <vector>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <msgpack.hpp>
#include "req.h"
#include "../io/external_io.h"
namespace redc
{
  struct Plugin
  {
    virtual bool poll_request(Request& req) = 0;
    virtual void post_request(Request const& res,
                              bool reliable = true) noexcept = 0;
  };

  struct Msgpack_Plugin : public Plugin
  {
    Msgpack_Plugin(std::unique_ptr<External_IO> io) noexcept;
    ~Msgpack_Plugin() noexcept = default;

    Msgpack_Plugin(Msgpack_Plugin&&) = default;
    Msgpack_Plugin(Msgpack_Plugin const&) noexcept = delete;

    Msgpack_Plugin& operator=(Msgpack_Plugin&&) = default;
    Msgpack_Plugin& operator=(Msgpack_Plugin const&) noexcept = delete;

    bool poll_request(Request& req) override;
    void post_request(Request const& res,
                      bool reliable = true) noexcept override;

    inline External_IO& io() noexcept { return *this->io_; }
    inline External_IO const& io() const noexcept { return *this->io_; }
  private:
    std::unique_ptr<External_IO> io_;
    msgpack::unpacker unpacker_;
    std::queue<msgpack::object_handle> raw_reqs_;
  };
}
