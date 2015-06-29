/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include <vector>
#include "../common/aabb.h"
#include "../gfx/mesh_chunk.h"
namespace game
{
  struct Structure
  {
    // We request to own the mesh chunk so we can give out weak-ptr like
    // instances ourselves.
    Structure(Mesh_Chunk&& m, AABB aabb, std::string name,
              std::string desc) noexcept;

    Structure(Structure&&) = default;
    Structure& operator=(Structure&&) = default;

    virtual ~Structure() noexcept {}

    Mesh_Chunk const& mesh_chunk() const noexcept;
    AABB aabb() const noexcept { return aabb_; }

    std::string name() const noexcept { return name_; }
    std::string desc() const noexcept { return desc_; }
  private:
    Mesh_Chunk mesh_chunk_;
    AABB aabb_;
    std::string name_;
    std::string desc_;
  };

  glm::vec3 ray_to_structure_bottom_center(Structure const& s) noexcept;

  std::vector<Structure> load_structures(std::string filename,
                                         Maybe_Owned<Mesh> mesh) noexcept;
}
