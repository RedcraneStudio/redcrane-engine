/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "map.h"
#include "../gfx/idriver.h"
namespace strat
{
  struct Json_Structure : public IStructure
  {
    Json_Structure(gfx::IDriver&, std::string, Mesh_Container&) noexcept;

    AABB aabb() const noexcept override;

    gfx::Material mat() const noexcept override;
    mesh_id_t mesh_id() const noexcept override;
  private:
    mesh_id_t mesh_id_;
    gfx::Material mat_;
    AABB aabb_;
  };
}
