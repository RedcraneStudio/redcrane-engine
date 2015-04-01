/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "object.h"
namespace survive
{
  namespace gfx
  {
    void Object::render() const noexcept
    {
      material->use();
      mesh->render();
    }
  }
}
