/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "triangle_conversion.h"
namespace redc { namespace collis
{
  void append_triangles(std::vector<Triangle>& triangles,
                        Indexed_Mesh_Data const& data, bool ccw) noexcept
  {
    // Ignore if there is some excess of vertex indices.
    auto old_size = triangles.size();
    auto new_elements = data.elements.size() / 3;
    triangles.resize(old_size + new_elements);

    for(unsigned int i = 0; i < new_elements; ++i)
    {
      auto vertex0 = data.vertices[data.elements[i * 3 + 0]];
      auto vertex1 = data.vertices[data.elements[i * 3 + 1]];
      auto vertex2 = data.vertices[data.elements[i * 3 + 2]];

      auto ind = old_size + i;

      triangles[ind].positions[0] = vertex0.position;
      triangles[ind].positions[1] = vertex1.position;
      triangles[ind].positions[2] = vertex2.position;

      // Recalculate normals, (not sure if this is necessarily a good idea).
      if(ccw)
      {
        auto a = vertex2.position - vertex1.position;
        auto b = vertex0.position - vertex1.position;
        triangles[ind].normal = glm::normalize(glm::cross(a, b));
      }
      else
      {
        auto a = vertex0.position - vertex1.position;
        auto b = vertex2.position - vertex1.position;
        triangles[ind].normal = glm::normalize(glm::cross(a, b));
      }

      // Calculate plane constant of this triangle.
      triangles[ind].plane_constant =
        glm::dot(triangles[ind].normal, -vertex0.position);
    }
  }
} }
