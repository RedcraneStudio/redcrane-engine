/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../imesh.h"
namespace redc { namespace gfx
{
  void allocate_standard_mesh_buffers(int vertices, int elements, IMesh& m,
                                      Usage_Hint, Upload_Hint) noexcept;
  void allocate_standard_mesh_buffers(int vertices, IMesh& m, Usage_Hint us,
                                      Upload_Hint up) noexcept;
} }
