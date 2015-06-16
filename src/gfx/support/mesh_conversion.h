/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../mesh_data.h"
namespace game { namespace gfx
{
  Indexed_Mesh_Data
  to_indexed_mesh_data(Indexed_Split_Mesh_Data const& data) noexcept;
} }
