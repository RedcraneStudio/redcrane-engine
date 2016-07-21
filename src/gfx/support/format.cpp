/*!
 * Copyright (C) Luke San Antonio
 * All rights reserved.
 */
#include "format.h"
namespace redc { namespace gfx
{
  void format_standard_mesh_buffers(IMesh& m) noexcept
  {
    m.format_buffer(m.get_buffer(0), 0, 3, Buffer_Format::Float, 0, 0);
    m.enable_vertex_attrib(0);

    m.format_buffer(m.get_buffer(1), 1, 3, Buffer_Format::Float, 0, 0);
    m.enable_vertex_attrib(1);

    m.format_buffer(m.get_buffer(2), 2, 2, Buffer_Format::Float, 0, 0);
    m.enable_vertex_attrib(2);
  }
} }
