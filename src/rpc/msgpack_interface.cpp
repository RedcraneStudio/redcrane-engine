/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "msgpack_interface.h"
#include "../common/result.h"
#include "../common/log.h"

#include <vector>

#include <msgpack.hpp>
namespace redc { namespace rpc
{
  Msgpack_Interface::Msgpack_Interface(Maybe_Owned<External_IO> io) noexcept
                                       : io_(std::move(io))
  {
    io_->set_read_callback([this](buf_t const& buf)
    {
      unpacker_.reserve_buffer(buf.size());
      std::memcpy(unpacker_.buffer(), &buf[0], buf.size());
      unpacker_.buffer_consumed(buf.size());

      msgpack::unpacked unpacked;
      while(unpacker_.next(&unpacked))
      {
        raw_reqs_.push(std::move(unpacked));
      }
    });
  }

  enum class Req_Err
  {
    NoArray,
    InvalidArraySize,
    BadFnType,
    BadParamsType,
    BadIdType,
    Unknown
  };

  Result<bool, Req_Err>
    try_make_request(Request& req, msgpack::object const& obj) noexcept
  {
    if(obj.type != msgpack::type::ARRAY)
    {
      return err(Req_Err::NoArray);
    }

    // So far so good, we have an array. It must have between 1 and 3 elements
    // (inclusive).
    if(obj.via.array.size < 1 || 3 < obj.via.array.size)
    {
      return err(Req_Err::InvalidArraySize);
    }

    // The first element must be an unsigned, positive integer.

    if(obj.via.array.ptr[0].type != msgpack::type::POSITIVE_INTEGER)
    {
      return err(Req_Err::BadFnType);
    }
    fn_t fn = obj.via.array.ptr[0].via.u64;

    // If we have a second parameter
    if(obj.via.array.size >= 2)
    {
      // The second element must either be an array or an id
      if(obj.via.array.ptr[1].type == msgpack::type::ARRAY)
      {
        Params p;
        p.object = msgpack::clone(obj.via.array.ptr[1]);

        // Do we have a third parameter?
        if(obj.via.array.size >= 3)
        {
          if(obj.via.array.ptr[2].type != msgpack::type::POSITIVE_INTEGER)
          {
            return err(Req_Err::BadIdType);
          }

          req.fn = fn;
          req.id = obj.via.array.ptr[2].via.u64;

          req.params = std::move(p);
          return ok(true);
        }
        else
        {
          // That's that, no request id
          req.fn = fn;
          req.id = boost::none;
          req.params = std::move(p);
          return ok(true);
        }
      }
      else if(obj.via.array.ptr[1].type == msgpack::type::POSITIVE_INTEGER)
      {
        // Get the id then we are done, the client can't put params after the
        // id.

        req.fn = fn;
        req.id = obj.via.array.ptr[1].via.u64;
        req.params = boost::none;
        return ok(true);
      }
      else
      {
        return err(Req_Err::BadParamsType);
      }
    }
    // Just use the function id and assume no parameters, etc.
    else
    {
      req.fn = fn;
      req.id = boost::none;
      req.params = boost::none;
      // This is a little redundant since we know if there wasn't an error that
      // it was successful. That's alright for now, I feel.
      return ok(true);
    }
  }

  bool Msgpack_Interface::poll_request(Request& req)
  {
    io_->step();

    // If we have one...
    if(raw_reqs_.size())
    {
      // make sense of it and try to return it.
      msgpack::object obj = raw_reqs_.front().get();

      bool ret = false;

      auto req_res = try_make_request(req, obj);
      // Can we guarantee that the second condition will not be evaluated as
      // long as the first one is false?
      if(req_res.ok() && *req_res.ok())
      {
        ret = true;
      }

      // Don't forget to pop the front object!
      raw_reqs_.pop();
      return ret;
    }

    // Otherwise just bail out
    return false;
  }

  void Msgpack_Interface::post_request(Request const& res,
                                    bool reliable) noexcept
  {
    std::ostringstream buf;
    msgpack::packer<std::ostringstream> packer(buf);

    if(res.params && res.id)
    {
      if(res.params->object.get().type != msgpack::type::ARRAY)
      {
        // What do we do?
        REDC_LOG_E("Response object with id % has bad params object.");
      }

      packer.pack_array(3);
      packer.pack(res.fn);
      packer.pack(res.params->object.get());
      packer.pack(*res.id);
    }
    else if(res.params && !res.id)
    {
      packer.pack_array(2);
      packer.pack(res.fn);
      packer.pack(res.params->object.get());
    }
    else if(!res.params && res.id)
    {
      packer.pack_array(2);
      packer.pack(res.fn);
      packer.pack(*res.id);
    }
    // They both must be missing
    else
    {
      packer.pack_array(1);
      packer.pack(res.fn);
    }
    io_->set_reliable(reliable);
    io_->write(buf_from_string(buf.str()));
    io_->step();
  }
} }
