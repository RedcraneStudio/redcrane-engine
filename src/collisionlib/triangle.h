/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <array>
#include <glm/glm.hpp>
namespace game { namespace collis
{
  struct Triangle
  {
    std::array<glm::vec3, 3> positions;
    glm::vec3 normal;

    float plane_constant;
  };

  inline float plane_constant(glm::vec3 pt, glm::vec3 norm) noexcept
  {
    return glm::dot(norm, -pt);
  }

  inline Triangle make_triangle(std::array<glm::vec3, 3> pts,
                                glm::vec3 normal) noexcept
  {
    return Triangle{pts, normal, plane_constant(pts[0], normal)};
  }

  inline float plane_signed_distance(Triangle const& t, glm::vec3 pt) noexcept
  {
    return glm::dot(t.normal, pt) + t.plane_constant;
  }

  // TODO: Combine these so we are not recalculating barycentric coordinates
  // twice.
  bool is_contained(Triangle const& t, glm::vec3 pt) noexcept;
  glm::vec3 to_barycentric_coord(Triangle const& t, glm::vec3 pos) noexcept;
} }
