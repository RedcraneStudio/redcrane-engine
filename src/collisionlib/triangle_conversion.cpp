/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "triangle_conversion.h"
namespace game
{
  std::vector<Triangle>
    triangles_from_mesh_data(Indexed_Mesh_Data const& data) noexcept
  {
    // Ignore if there is some excess of vertex indices.
    auto triangles = std::vector<Triangle>(data.elements.size() / 3);

    for(unsigned int i = 0; i < triangles.size(); ++i)
    {
      auto vertex0 = data.vertices[data.elements[i * 3 + 0]];
      auto vertex1 = data.vertices[data.elements[i * 3 + 1]];
      auto vertex2 = data.vertices[data.elements[i * 3 + 2]];

      triangles[i].positions[0] = vertex0.position;
      triangles[i].positions[1] = vertex1.position;
      triangles[i].positions[2] = vertex2.position;

      auto a = vertex2.position - vertex1.position;
      auto b = vertex0.position - vertex1.position;

      triangles[i].normal = glm::normalize(glm::cross(a, b));
    }

    return triangles;
  }
}
