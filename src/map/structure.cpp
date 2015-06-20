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

  Structure_Instance::Structure_Instance(Structure& s) noexcept : s_type_(&s){}

  void Structure_Instance::set_structure_type(Structure const& s) noexcept
  {
    s_type_ = &s;
  }
  Structure const& Structure_Instance::structure() noexcept
  {
    return *s_type_;
  }
}
