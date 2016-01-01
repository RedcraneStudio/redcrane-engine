/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "triangle.h"
#include "../gfx/mesh_data.h"
namespace redc { namespace collis
{
  void append_triangles(std::vector<Triangle>& triangles,
                        Indexed_Mesh_Data const& data, bool ccw=true) noexcept;
} }
