/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "req.h"
#include <boost/optional.hpp>
namespace redc { namespace rpc
{
  struct Run_Context
  {
    bool should_make_params = true;

    bool is_error = false;
    boost::optional<Params> response = boost::none;

    void* userdata = nullptr;
  };

  using method_t = std::function<void(Run_Context*, Params& params)>;

  // Request => Function call
  void dispatch(std::vector<method_t> const& methods, Request req,
                Run_Context* ctx = nullptr) noexcept;

  boost::optional<rpc::Request>
    dispatch_request(rpc::Request req,std::vector<rpc::method_t> const& methods,
                     void* userdata) noexcept;
} }
