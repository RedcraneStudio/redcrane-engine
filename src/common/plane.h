/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <glm/glm.hpp>
namespace redc
{
  struct Plane
  {
    glm::vec3 normal;
    float dist;
  };
  inline glm::vec4 plane_as_vec4(Plane const& pl) noexcept
  {
    return glm::vec4(pl.normal, -pl.dist);
  }

  inline float signed_distance(Plane const& plane, glm::vec3 const& pt)
  {
    return glm::dot(plane.normal, pt) + plane.dist;
  }

  inline glm::vec3 project_onto_plane(Plane const& plane, glm::vec3 const& pt)
  {
    return pt - signed_distance(plane, pt) * plane.normal;
  }

  glm::vec3 intercept_plane(Plane pl, glm::vec3 start, glm::vec3 dir)
  {
    float t = (-pl.dist - glm::dot(pl.normal, start)) / dot(pl.normal, dir);

    return start + (t * dir);
  }
}
