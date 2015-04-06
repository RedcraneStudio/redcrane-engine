/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "map.h"
namespace strat
{
  struct Json_Structure : public IStructure
  {
    Json_Structure(std::string json, Mesh_Container&) noexcept;

    AABB aabb() const noexcept override;

    mesh_id_t mesh_id() const noexcept override;
  private:
    mesh_id_t mesh_id_;
    AABB aabb_;
  };
}
