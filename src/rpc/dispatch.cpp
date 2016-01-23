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

  boost::optional<rpc::Request>
    dispatch_request(rpc::Request req,std::vector<rpc::method_t> const& methods,
                     void* userdata) noexcept
  {
    rpc::Run_Context ctx;

    if(req.id) ctx.should_make_params = true;
    else ctx.should_make_params = false;

    ctx.userdata = userdata;

    // Dispatch those requests
    rpc::dispatch(methods, req, &ctx);

    // If we don't need a response or we don't have one, move on
    if(!req.id || !ctx.response) return boost::none;
    else if(ctx.response->object.get().type == msgpack::type::ARRAY)
    {
      // If the request has an id and the function returned some parameters
      // we should send them back.

      // Send back a response if necessary.

      rpc::Request response;
      response.id = req.id;
      response.params = std::move(*ctx.response);
      if(ctx.is_error)
      {
        response.fn = 1;
      }
      else
      {
        response.fn = 0;
      }

      return response;
    }
    return boost::none;
  }
} }
