/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "req.h"
namespace redc { namespace rpc
{
  // I believe msgpack::object_handle has implicit move constructors which
  // scare me but okay.

  Params::Params() noexcept {}
  Params::Params(msgpack::object const& obj,
                 std::unique_ptr<msgpack::zone> zone) noexcept
    : object(obj, std::move(zone)) {}
  Params::Params(msgpack::object_handle&& obj) noexcept
    : object(std::move(obj)) {}

  Params::Params(Params const& p) noexcept
    : object(msgpack::clone(p.object.get())) {}
  Params::Params(Params&& p) noexcept
    : object(std::move(p.object)) {}

  Params& Params::operator=(Params const& p) noexcept
  {
    this->object = msgpack::clone(p.object.get());
    return *this;
  }
  Params& Params::operator=(Params&& p) noexcept
  {
    this->object = std::move(p.object);
    return *this;
  }
} }
