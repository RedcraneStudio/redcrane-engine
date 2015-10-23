/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "allocate.h"
#include "../mesh_data.h"
namespace game { namespace gfx
{
  void allocate_standard_mesh_buffers(int vertices, int elements, Mesh& m,
                                      Usage_Hint us, Upload_Hint up) noexcept
  {
    m.allocate_buffer(vertices * sizeof(Vertex::position), us,up);
    m.allocate_buffer(vertices * sizeof(Vertex::normal), us,up);
    m.allocate_buffer(vertices * sizeof(Vertex::uv), us, up);

    m.allocate_element_array(elements, us, up);
  }
  void allocate_standard_mesh_buffers(int vertices, Mesh& m, Usage_Hint us,
                                      Upload_Hint up) noexcept
  {
    m.allocate_buffer(vertices * sizeof(Vertex::position), us, up);
    m.allocate_buffer(vertices * sizeof(Vertex::normal), us, up);
    m.allocate_buffer(vertices * sizeof(Vertex::uv), us, up);
  }
} }
