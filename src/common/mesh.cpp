/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "mesh.h"
namespace game
{
  AABB generate_aabb(Mesh_Data const& mesh) noexcept
  {
    glm::vec3 min;
    glm::vec3 max;

    for(auto const& vertex : mesh.vertices)
    {
      min.x = std::min(min.x, vertex.position.x);
      min.y = std::min(min.y, vertex.position.y);
      min.z = std::min(min.z, vertex.position.z);

      max.x = std::max(max.x, vertex.position.x);
      max.y = std::max(max.y, vertex.position.y);
      max.z = std::max(max.z, vertex.position.z);
    }

    auto aabb = AABB{};
    aabb.width = std::abs(max.x - min.x);
    aabb.height = std::abs(max.y - min.y);
    aabb.depth = std::abs(max.z - min.z);

    aabb.min = min;

    return aabb;
  }
}
