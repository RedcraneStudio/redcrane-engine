/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <msgpack.hpp>
namespace redc
{
  enum class Primitive_Type
  {
    Triangle, Line,
    Triangle_Fan
  };
}
MSGPACK_ADD_ENUM(redc::Primitive_Type);
