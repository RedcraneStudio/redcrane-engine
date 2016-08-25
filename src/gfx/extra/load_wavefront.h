/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <istream>
#include <string>
#include "../mesh_data.h"
namespace redc { namespace gfx
{
  Vert_Ref parse_wavefront_vert_ref(std::string str) noexcept;

  Indexed_Split_Mesh_Data load_wavefront(std::string obj) noexcept;
  Indexed_Split_Mesh_Data load_wavefront(std::istream& stream) noexcept;
} }
