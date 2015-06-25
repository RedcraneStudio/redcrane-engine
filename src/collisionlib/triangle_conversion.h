/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "triangle.h"
#include "../gfx/mesh_data.h"
namespace game
{
  // Assumes triangles are given in ccw order.
  std::vector<Triangle>
    triangles_from_mesh_data(Indexed_Mesh_Data const&) noexcept;
}
