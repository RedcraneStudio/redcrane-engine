/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "triangle.h"
namespace redc { namespace collis
{
  void inplace_make_espace(std::vector<Triangle>& triangles,
                           glm::vec3 const& scale_factor) noexcept
  {
    // Scale each triangle by scale_factor and recalculate normal and plane
    // constant
    for(auto& triangle : triangles)
    {
      // TODO: Do some loop unrolling?
      for(auto& pt : triangle.positions)
      {
        pt.x *= scale_factor.x;
        pt.y *= scale_factor.y;
        pt.z *= scale_factor.z;
      }

      // Recalculate normal
      auto a = triangle.positions[2] - triangle.positions[1];
      auto b = triangle.positions[0] - triangle.positions[1];
      auto norm_cross = glm::normalize(glm::cross(a, b));
      // If this winding order is the same direction as the normal, use it.
      if(glm::dot((norm_cross = glm::normalize(glm::cross(a, b))),
                  triangle.normal) > 0.0)
      {
        triangle.normal = norm_cross;
      }
      else
      {
        triangle.normal = glm::normalize(glm::cross(b, a));
      }

      triangle.plane_constant =
        plane_constant(triangle.positions[0], triangle.normal);
    }
  }
  std::vector<Triangle> make_espace(std::vector<Triangle> const& tris,
                                    glm::vec3 const& scale_factor) noexcept
  {
    std::vector<Triangle> ret(tris.begin(), tris.end());
    inplace_make_espace(ret, scale_factor);
    return ret;
  }
  bool espace_is_intersecting(Triangle const& t, glm::vec3 pt) noexcept
  {
    // Find the signed distance between the point and the plane of the triangle
    auto dist = plane_signed_distance(t, pt);
    if(dist > 1.0f)
    {
      return false;
    }

    // If we are here it means we are definitely intersecting with the plane
    // of the triangle, so lets see if we intersect the triangle.
    for(auto tri_pt : t.positions)
    {
      auto length = glm::length(tri_pt - pt);
      if(length < 1.0f)
      {
        // We found at least one triangle point that we are intersecting with.
        return true;
      }
    }

    return false;
  }

  bool is_contained(Triangle const& t, glm::vec3 pt) noexcept
  {
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
} }
