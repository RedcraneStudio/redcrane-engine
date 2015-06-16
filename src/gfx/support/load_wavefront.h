/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include "../mesh_data.h"
namespace game { namespace gfx
{
  Indexed_Split_Mesh_Data load_wavefront(std::string obj) noexcept;
} }
