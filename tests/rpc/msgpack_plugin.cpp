/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
//#include <boost/optional/optional_io.hpp>
#include "catch/catch.hpp"
#include "io/external_io.h"
#include "rpc/plugins.h"
#include "rpc/dispatch.h"
#include "common/utility.h"

TEST_CASE("Msgpack plredcin poll_request", "[rpclib]")
{
  auto out_pipe = std::make_unique<redc::Pipe_IO>();
  auto& write_pipe = out_pipe->counterpart();

  redc::Msgpack_Plugin plugin{std::move(out_pipe)};

  using namespace redc::literals;

  // Json: [0]
  write_pipe.write("\x91\x00"_buf);

  // Json: [0, []]
  write_pipe.write("\x92\x00\x90"_buf);

  // Json: [0, [], 1]
  write_pipe.write("\x93\x00\x90\x01"_buf);

  // Json: [2, ["Hello"], 1]
  write_pipe.write("\x93\x02\x91\xa5\x48\x65\x6c\x6c\x6f\x01"_buf);

  // Json: [2, ["Hello"]]
  write_pipe.write("\x92\x02\x91\xa5\x48\x65\x6c\x6c\x6f"_buf);

  // Json: [3, 2]
  write_pipe.write("\x92\x03\x02"_buf);

  write_pipe.step();

  redc::Request req;
  REQUIRE(plugin.poll_request(req));
  CHECK(req.fn == 0);
  CHECK_FALSE(req.id);
  CHECK_FALSE(req.params);

  REQUIRE(plugin.poll_request(req));
  CHECK(req.fn == 0);
  CHECK_FALSE(req.id);
  CHECK(req.params);
  REQUIRE(req.params->object.get().type == msgpack::type::ARRAY);
  REQUIRE(req.params->object.get().via.array.size == 0);

  REQUIRE(plugin.poll_request(req));
  CHECK(req.fn == 0);
  CHECK(req.id);
  CHECK(req.params);
  REQUIRE(req.params->object.get().type == msgpack::type::ARRAY);
  REQUIRE(req.params->object.get().via.array.size == 0);

  REQUIRE(plugin.poll_request(req));
  CHECK(req.fn == 2);
  CHECK(req.id);
  CHECK(req.params);
  REQUIRE(req.params->object.get().type == msgpack::type::ARRAY);
  REQUIRE(req.params->object.get().via.array.size == 1);
  REQUIRE(req.params->object.get().via.array.ptr[0].type == msgpack::type::STR);

  REQUIRE(plugin.poll_request(req));
  CHECK(req.fn == 2);
  CHECK_FALSE(req.id);
  CHECK(req.params);
  REQUIRE(req.params->object.get().type == msgpack::type::ARRAY);
  REQUIRE(req.params->object.get().via.array.size == 1);
  REQUIRE(req.params->object.get().via.array.ptr[0].type == msgpack::type::STR);

  REQUIRE(plugin.poll_request(req));
  CHECK(req.fn == 3);
  CHECK(req.id);
  CHECK_FALSE(req.params);
}
TEST_CASE("Msgpack plugin post_request", "[rpclib]")
{
  auto plugin_pipe = std::make_unique<redc::Pipe_IO>();
  auto& read_pipe = plugin_pipe->counterpart();

  std::vector<redc::uchar> buf_recieved_;
  size_t where = 0;

  read_pipe.set_read_callback([&](auto const& buf)
  {
    for(redc::uchar c : buf)
    {
      buf_recieved_.push_back(c);
    }
  });

  auto read_bytes = [&](size_t count) -> std::vector<redc::uchar>
  {
    read_pipe.step();

    REQUIRE(where + count <= buf_recieved_.size());

    auto begin = std::begin(buf_recieved_) + where;
    auto end = begin + count;
    where += count;

    return std::vector<redc::uchar>(begin, end);
  };

  using namespace redc::literals;

  redc::Msgpack_Plugin plugin{std::move(plugin_pipe)};

  // [0]
  redc::Request req;
  req.fn = 0;
  plugin.post_request(req);
  REQUIRE("\x91\x00"_buf == read_bytes(2));

  // [1]
  req.fn = 1;
  plugin.post_request(req);
  REQUIRE("\x91\x01"_buf == read_bytes(2));

  // [1, 5]
  req.id = 5;
  plugin.post_request(req);
  REQUIRE("\x92\x01\x05"_buf == read_bytes(3));

  // [1, 6]
  req.id = 6;
  plugin.post_request(req);
  REQUIRE("\x92\x01\x06"_buf == read_bytes(3));

  // [1, [3]]
  req.id = boost::none;
  req.params = redc::make_params(3);
  plugin.post_request(req);
  REQUIRE("\x92\x01\x91\x03"_buf == read_bytes(4));

  // [1, [5, 2, 5]]
  req.params = redc::make_params(5, 2, 5);
  plugin.post_request(req);
  REQUIRE("\x92\x01\x93\x05\x02\x05"_buf == read_bytes(6));

  // [1, [5, 2, 5], 7]
  req.id = 7;
  plugin.post_request(req);
  REQUIRE("\x93\x01\x93\x05\x02\x05\x07"_buf == read_bytes(7));

  // [1, [5, 2, 5], 2]
  req.id = 2;
  plugin.post_request(req);
  REQUIRE("\x93\x01\x93\x05\x02\x05\x02"_buf == read_bytes(7));
}
TEST_CASE("Request dispatcher", "[rpclib]")
{
  int var0 = 0;
  int var1 = 0;

  auto func0 = [&var0](auto* run_context, auto& params)
  {
    var0 = 5;
  };
  auto func1 = [&var1](auto* run_context, auto& params)
  {
    ++var1;
  };

  std::vector<redc::method_t> methods{func0, func1};

  auto in = 5;
  redc::Request req;
  req.fn = 0;
  req.params = redc::make_params(5);
  redc::dispatch(methods, req);
  REQUIRE(var0 == in);

  req.fn = 1;
  req.params = boost::none;

  redc::dispatch(methods, req);
  redc::dispatch(methods, req);
  REQUIRE(var1 == 2);
}
