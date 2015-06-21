/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "structure.h"
namespace game
{
  Structure::Structure(Mesh_Chunk&& m, AABB aabb, std::string name,
                      std::string desc) noexcept
    : mesh_chunk_(std::move(m)), aabb_(aabb), name_(name), desc_(desc) { }

  Mesh_Chunk const& Structure::mesh_chunk() const noexcept
  {
    return mesh_chunk_;
  }
}
