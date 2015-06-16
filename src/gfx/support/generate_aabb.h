/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../mesh_data.h"
#include "../../common/aabb.h"
namespace game { namespace gfx
{
  AABB generate_aabb(Indexed_Mesh_Data const&) noexcept;
  AABB generate_aabb(Indexed_Split_Mesh_Data const&) noexcept;

  AABB generate_aabb(Ordered_Mesh_Data const&) noexcept;
  AABB generate_aabb(Ordered_Split_Mesh_Data const&) noexcept;
} }
