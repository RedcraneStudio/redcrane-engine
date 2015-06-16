/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved
 */
#include "mesh_chunk.h"
namespace game { namespace gfx
{
  void render_chunk(Mesh_Chunk const& m) noexcept
  {
    if(!m.mesh) return;

    m.mesh->set_primitive_type(m.type);

    if(m.base_vertex)
    {
      if(m.base_vertex.value() == 0)
      {
        m.mesh->draw_elements(m.start, m.count);
      }
      else
      {
        m.mesh->draw_elements_base_vertex(m.start, m.count,
                                          m.base_vertex.value());
      }
    }
    else
    {
      m.mesh->draw_arrays(m.start, m.count);
    }
  }
} }
