/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <array>
#include <vector>
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

  /*!
   * Scales every triangle in the given vector so that an object with scales of
   * scale_factor is equivalent to a unit sphere in relation to the resulting
   * triangles.
   */
  void inplace_make_espace(std::vector<Triangle>& triangles,
                           glm::vec3 const& scale_factor) noexcept;
  std::vector<Triangle> make_espace(std::vector<Triangle> const& tris,
                                    glm::vec3 const& scale_factor) noexcept;

  // We assume the point is the position of a unit sphere.
  // The triangle should have been converted to this "elliptical space"
  // mentioned here: http://www.peroxide.dk/papers/collision/collision.pdf
  bool espace_is_intersecting(Triangle const& t, glm::vec3 pt) noexcept;

  // TODO: Combine these so we are not recalculating barycentric coordinates
  // twice.
  bool is_contained(Triangle const& t, glm::vec3 pt) noexcept;
  glm::vec3 to_barycentric_coord(Triangle const& t, glm::vec3 pos) noexcept;
} }
