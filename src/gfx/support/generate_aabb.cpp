/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "generate_aabb.h"
#include <glm/glm.hpp>

namespace game { namespace gfx
{
  AABB generate_aabb(Indexed_Mesh_Data const& mesh) noexcept
  {
    glm::vec3 min, max;

    using std::begin; using std::end;
    for(auto const& v : mesh.vertices)
    {
      auto const& pos = v.position;

      min = min_pt(min, pos);
      max = max_pt(max, pos);
    }

    return aabb_from_min_max(min, max);
  }
  AABB generate_aabb(Indexed_Split_Mesh_Data const& mesh) noexcept
  {
    glm::vec3 min, max;

    using std::begin; using std::end;
    for(auto const& pos : mesh.positions)
    {
      min = min_pt(min, pos);
      max = max_pt(max, pos);
    }

    return aabb_from_min_max(min, max);
  }

  AABB generate_aabb(Ordered_Mesh_Data const& mesh) noexcept
  {
    glm::vec3 min, max;

    using std::begin; using std::end;
    for(auto const& v : mesh.vertices)
    {
      auto const& pos = v.position;

      min = min_pt(min, pos);
      max = max_pt(max, pos);
    }

    return aabb_from_min_max(min, max);
  }
  AABB generate_aabb(Ordered_Split_Mesh_Data const& mesh) noexcept
  {
    glm::vec3 min, max;

    using std::begin; using std::end;
    for(auto const& pos : mesh.positions)
    {
      min = min_pt(min, pos);
      max = max_pt(max, pos);
    }

    return aabb_from_min_max(min, max);
  }
} }
