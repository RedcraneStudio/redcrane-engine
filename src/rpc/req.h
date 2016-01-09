/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once

#include <string>
#include <boost/optional.hpp>
#include <msgpack.hpp>

namespace redc { namespace rpc
{
  using boost::optional;

  // Functions and ids just happen to have the same type for now.
  using fn_t = uint16_t;
  using id_t = uint16_t;

  // We need to keep the msgpack zone around to keep the memory alive so be it.
  struct Params
  {
    Params() noexcept;
    Params(msgpack::object const& obj,
           std::unique_ptr<msgpack::zone> zone) noexcept;
    Params(msgpack::object_handle&& obj) noexcept;

    Params(Params const& p) noexcept;
    Params(Params&& p) noexcept;

    Params& operator=(Params const& p) noexcept;
    Params& operator=(Params&& p) noexcept;

    msgpack::object_handle object;
  };

  template <std::size_t Where>
  void build_params(msgpack::zone& zone, msgpack::object& p) noexcept
  {
    return;
  }

  template <std::size_t Where, class Arg0, class... ArgN>
  void build_params(msgpack::zone& zone, msgpack::object& p,
                    Arg0&& first_arg, ArgN&&... args) noexcept
  {
    // We assume here that the object already contains an allocated array
    // that has the right size.

    // Set the object wherever we are to that argument.
    p.via.array.ptr[Where] = msgpack::object(first_arg, zone);

    // Then do it again for the next element and the next argument
    build_params<Where + 1>(zone, p, std::forward<ArgN>(args)...);
  }

  template <class... Args>
  Params make_params(Args&&... args) noexcept
  {
    msgpack::zone zone;
    msgpack::object params_obj;

    // This code is based on code from msgpack/adaptor/vector.hpp.
    params_obj.type = msgpack::type::ARRAY;
    params_obj.via.array.ptr = static_cast<msgpack::object*>(
      zone.allocate_align(sizeof(msgpack::object) * sizeof...(Args)));
    params_obj.via.array.size = sizeof...(Args);

    build_params<0>(zone, params_obj, std::forward<Args>(args)...);

    return {params_obj, std::make_unique<msgpack::zone>(std::move(zone))};
  }

  struct Request
  {
    // Every request has a function,
    fn_t fn;

    // an optional id to track the response,
    optional<id_t> id;

    // and parameters.
    optional<Params> params;
  };
} }
