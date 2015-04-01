/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "iscene_observer.h"
namespace survive
{
  namespace gfx
  {
    struct IMaterial : public IScene_Observer
    {
      virtual ~IMaterial() noexcept {}
      virtual void use() const noexcept = 0;

      void set_projection(glm::mat4 const& proj) noexcept = 0;
      void set_view(glm::mat4 const& view) noexcept = 0;
    };
  }
}
