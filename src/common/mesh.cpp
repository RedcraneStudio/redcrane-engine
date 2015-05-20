/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "mesh.h"
namespace game
{
  Mesh_Chunk Mesh_Data::append(Mesh_Data const& md) noexcept
  {
    // Should we check to make sure hints and primitive type are the same? Ehh.

    vertices.insert(vertices.end(), md.vertices.begin(), md.vertices.end());

    auto initial_size = elements.size();
    for(unsigned int index : md.elements)
    {
      elements.push_back(index + initial_size);
    }

    Mesh_Chunk chunk;
    chunk.offset = initial_size;
    chunk.count = md.elements.size();
    return chunk;
  }
  void Mesh::allocate_from(Mesh_Data&& mesh) noexcept
  {
    // By default initiate a copy of this data.
    allocate_from(static_cast<Mesh_Data const&>(mesh));
  }
  void Mesh::allocate_from(Mesh_Data const& md) noexcept
  {
    allocate(md.vertices.size(), md.elements.size(), md.usage_hint,
             md.upload_hint, md.primitive);
    set_vertices(0, md.vertices.size(), &md.vertices[0]);
    set_element_indices(0, md.elements.size(), &md.elements[0]);
    set_num_element_indices(md.elements.size());
  }
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
