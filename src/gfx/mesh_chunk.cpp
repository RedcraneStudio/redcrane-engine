/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved
 */
#include "mesh_chunk.h"
namespace game { namespace gfx
{
  Mesh_Chunk copy_mesh_chunk_share_mesh(Mesh_Chunk const& orig) noexcept
  {
    Mesh_Chunk ret;

    ret.start = orig.start;
    ret.count = orig.count;

    ret.type = orig.type;
    ret.mesh.set_pointer(orig.mesh.get());

    ret.base_vertex = orig.base_vertex;

    return ret;
  }
  Mesh_Chunk copy_mesh_chunk_move_mesh(Mesh_Chunk& orig) noexcept
  {
    Mesh_Chunk ret;

    ret.start = orig.start;
    ret.count = orig.count;

    ret.type = orig.type;

    ret.mesh = std::move(orig.mesh);
    orig.mesh.set_pointer(ret.mesh.get());

    ret.base_vertex = orig.base_vertex;

    return ret;
  }
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
