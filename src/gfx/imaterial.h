/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
namespace survive
{
  namespace gfx
  {
    struct IMaterial
    {
      virtual ~IMaterial() noexcept {}
      virtual void use() const noexcept = 0;
    };
  }
}
