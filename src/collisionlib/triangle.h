/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <array>
#include <glm/glm.hpp>
namespace game
{
  struct Triangle
  {
    std::array<glm::vec3, 3> positions;
    glm::vec3 normal;
  };

  // TODO: Combine these so we are not recalculating barycentric coordinates
  // twice.
  bool is_contained(Triangle const& t, glm::vec3 pt) noexcept;
  glm::vec3 to_barycentric_coord(Triangle const& t, glm::vec3 pos) noexcept;
}
