/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include "mesh.h"
#include "../gfx/prepared_mesh.h"
#include "../gfx/idriver.h"
#include "ID_Map.hpp"
namespace strat
{
  using mesh_id_t = int;
  struct Mesh_Container
  {
    Mesh_Container(gfx::IDriver& driver) noexcept : driver_(&driver) {}

    mesh_id_t load(std::string fn) noexcept;
    Mesh* mesh(mesh_id_t) noexcept;
    gfx::Prepared_Mesh* prepared_mesh(mesh_id_t) noexcept;
  private:
    ID_Map< std::shared_ptr<gfx::Prepared_Mesh>, mesh_id_t > meshes_;
    gfx::IDriver* driver_;
  };
}
