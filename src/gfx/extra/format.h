/*!
 * Copyright (C) Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <vector>
#include <memory>
#include "../imesh.h"
namespace redc { namespace gfx
{
  void format_standard_mesh_buffers(
    IMesh& mesh, std::vector<std::unique_ptr<IBuffer> >& buf
  );
} }
