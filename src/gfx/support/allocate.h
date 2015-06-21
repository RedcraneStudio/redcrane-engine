/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../mesh.h"
namespace game { namespace gfx
{
  void allocate_standard_mesh_buffers(int vertices, int elements, Mesh& m,
                                      Usage_Hint, Upload_Hint) noexcept;
} }
