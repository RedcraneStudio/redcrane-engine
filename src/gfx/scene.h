/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <glm/glm.hpp>
namespace survive
{
  namespace gfx
  {
    /*!
     * \brief Graphical related parameters of the scene.
     */
    struct Scene
    {
      glm::mat4 projection;
      glm::mat4 view;
    };
  }
}
