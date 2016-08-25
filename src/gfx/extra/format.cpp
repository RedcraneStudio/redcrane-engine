/*!
 * Copyright (C) Luke San Antonio
 * All rights reserved.
 */
#include "format.h"
#include "../ibuffer.h"
namespace redc { namespace gfx
{
  void format_standard_mesh_buffers(
    IMesh& m,
    std::vector<std::unique_ptr<IBuffer> >& bufs
  )
  {
    // Vertices
    Attrib_Bind bind = 0;
    m.format_buffer(*bufs[0], bind, Attrib_Type::Vec3, Data_Type::Float, 0, 0);
    m.enable_attrib_bind(bind);

    // Normals
    ++bind;
    m.format_buffer(*bufs[1], bind, Attrib_Type::Vec3, Data_Type::Float, 0, 0);
    m.enable_attrib_bind(bind);

    // UVs
    ++bind;
    m.format_buffer(*bufs[2], bind, Attrib_Type::Vec2, Data_Type::Float, 0, 0);
    m.enable_attrib_bind(bind);

    if(bufs.size() > 3)
    {
      // There is an element array buffer in there somewhere!
      m.use_element_buffer(*bufs[3], Data_Type::UInt);
    }
  }
} }
