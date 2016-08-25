/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved
 */
#include "mesh_chunk.h"
namespace redc { namespace gfx
{
  Mesh_Chunk copy_mesh_chunk_share_mesh(Mesh_Chunk const& orig) noexcept
  {
    Mesh_Chunk ret;

    ret.start = orig.start;
    ret.count = orig.count;

    ret.type = orig.type;

    // Reference the buffers of the original.
    for(Maybe_Owned<IBuffer> const& buf : orig.buffers)
    {
      ret.buffers.push_back(ref_mo(buf));
    }
    ret.mesh = ref_mo(orig.mesh);

    ret.base_vertex = orig.base_vertex;

    return ret;
  }
  Mesh_Chunk copy_mesh_chunk_move_mesh(Mesh_Chunk& orig) noexcept
  {
    Mesh_Chunk ret;

    ret.start = orig.start;
    ret.count = orig.count;

    ret.type = orig.type;

    ret.buffers = std::move(orig.buffers);
    ret.mesh = std::move(orig.mesh);

    ret.base_vertex = orig.base_vertex;

    return ret;
  }
  void render_chunk(Mesh_Chunk const& m) noexcept
  {
    if(!m.mesh) return;

    m.mesh->set_primitive_type(m.type);

    // Ahh this function is deprecated as of 1.56 I think
    if(m.base_vertex.get_value_or(0) == 0)
    {
      m.mesh->draw_elements(m.start, m.count);
    }
    else
    {
      m.mesh->draw_elements_base_vertex(m.start, m.count, m.base_vertex.get());
    }
  }
} }
