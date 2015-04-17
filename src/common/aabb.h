/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <glm/glm.hpp>
namespace game
{
  struct AABB
  {
    float depth;
    float height;
    float width;

    glm::vec3 min;
  };
}
