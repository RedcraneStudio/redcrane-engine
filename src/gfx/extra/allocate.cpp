/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "allocate.h"
#include "../ibuffer.h"
#include "../mesh_data.h"
namespace redc { namespace gfx
{
   void allocate_standard_mesh_buffers(
     int vertices, int elements,
     std::vector<std::unique_ptr<IBuffer> >& b,
     Usage_Hint us, Upload_Hint up)
  {
    // Allocate positions, normals, uvs
    allocate_standard_mesh_buffers(vertices, b, us, up);
    // Allocate element array buffer
    b[3]->allocate(Buffer_Target::Element_Array,
                   elements * sizeof(unsigned int),
                   nullptr, us, up);
  }
  void allocate_standard_mesh_buffers(
    int vertices, std::vector<std::unique_ptr<IBuffer> >& b,
    Usage_Hint us, Upload_Hint up)
  {
    b[0]->allocate(Buffer_Target::Array, vertices * sizeof(Vertex::position),
                  nullptr, us, up);
    b[1]->allocate(Buffer_Target::Array, vertices * sizeof(Vertex::normal),
                  nullptr, us, up);
    b[2]->allocate(Buffer_Target::Array, vertices * sizeof(Vertex::uv),
                  nullptr, us, up);
  }
} }
