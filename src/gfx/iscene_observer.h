/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <glm/glm.hpp>
namespace strat
{
  namespace gfx
  {
    struct IScene_Observer
    {
      virtual void set_projection(glm::mat4 const& proj) noexcept = 0;
      virtual void set_view(glm::mat4 const& view) noexcept = 0;
    };
  }
}

