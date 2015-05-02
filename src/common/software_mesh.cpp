/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "software_mesh.h"
namespace game
{
  void Software_Mesh::prepare(Mesh_Data const& data) noexcept
  {
    mesh_data_ = data;
  }
  void Software_Mesh::prepare(Mesh_Data&& data) noexcept
  {
    mesh_data_ = std::move(data);
  }
  void Software_Mesh::set_vertices(unsigned int begin, unsigned int length,
                                   Vertex const* src_vertices) noexcept
  {
    mesh_data_.set_vertices(begin, length, src_vertices);
  }
}
