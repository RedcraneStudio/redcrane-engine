/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "dispatch.h"
namespace redc { namespace rpc
{
  void dispatch(std::vector<method_t> const& methods, Request req,
                Run_Context* ctx) noexcept
  {
    if(req.fn >= methods.size())
    {
      // Get the hell outta here before we jump to mysterious code.
      return;
    }

    if(!req.params)
    {
      msgpack::object obj;
      obj.type = msgpack::type::ARRAY;
      obj.via.array.size = 0;
      obj.via.array.ptr = nullptr;

      req.params = Params{msgpack::clone(obj)};
    }
    (methods[req.fn])(ctx, *req.params);
  }
} }
