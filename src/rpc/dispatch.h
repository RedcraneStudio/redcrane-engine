/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "req.h"
namespace redc
{
  struct Run_Context
  {
    Run_Context() noexcept : is_error(false), response() {}
    Run_Context(bool err, Params res) noexcept : is_error(err), response(res){}

    bool is_error;
    Params response;
  };

  using method_t = std::function<void(Run_Context*, Params& params)>;

  void dispatch(std::vector<method_t>& methods, Request req,
                Run_Context* ctx = nullptr) noexcept;
}
