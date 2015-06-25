/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "triangle.h"
namespace game
{
  bool is_contained_xz(glm::vec2 pt2, Triangle t) noexcept
  {
    for(auto& pos : t.positions)
    {
      pos.y = 0.0f;
    }

    auto pt = glm::vec3(pt2.x, 0.0f, pt2.y);

    // Adapted from http://www.blackpawn.com/texts/pointinpoly/default.html
    auto v0 = t.positions[2] - t.positions[0];
    auto v1 = t.positions[1] - t.positions[0];
    auto v2 = pt - t.positions[0];

    // Compute dot products
    auto dot00 = glm::dot(v0, v0);
    auto dot01 = glm::dot(v0, v1);
    auto dot02 = glm::dot(v0, v2);
    auto dot11 = glm::dot(v1, v1);
    auto dot12 = glm::dot(v1, v2);

    // Compute barycentric coordinates
    auto invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
    auto u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    auto v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // Check if point is in triangle
    return (u >= 0) && (v >= 0) && (u + v < 1);
  }
  glm::vec3 to_barycentric_coord(Triangle const& t, glm::vec3 pt) noexcept
  {
    auto v0 = t.positions[2] - t.positions[0];
    auto v1 = t.positions[1] - t.positions[0];
    auto v2 = pt - t.positions[0];

    // Compute dot products
    auto dot00 = glm::dot(v0, v0);
    auto dot01 = glm::dot(v0, v1);
    auto dot02 = glm::dot(v0, v2);
    auto dot11 = glm::dot(v1, v1);
    auto dot12 = glm::dot(v1, v2);

    // Compute barycentric coordinates
    auto invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
    auto u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    auto v = (dot00 * dot12 - dot01 * dot02) * invDenom;
    auto w = 1 - u - v;
    return glm::vec3(u, v, w);
  }
}
