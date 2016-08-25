/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <vector>
#include "../imesh.h"
namespace redc { namespace gfx
{
  void allocate_standard_mesh_buffers(int vertices, int elements,
                                      std::vector<std::unique_ptr<IBuffer> >& b,
                                      Usage_Hint, Upload_Hint);
  void allocate_standard_mesh_buffers(int vertices,
                                      std::vector<std::unique_ptr<IBuffer> >& b,
                                      Usage_Hint us, Upload_Hint up);
} }
