/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../../common/maybe_owned.hpp"
#include "../mesh_chunk.h"
#include "../mesh_data.h"
#include "../imesh.h"
#include "../../common/log.h"
namespace redc { namespace gfx
{
  void write_vertices_to_buffer(std::vector<Vertex> const& vertices,
                                Mesh_Chunk& chunk, std::size_t elemnt_offset);
  void write_element_array_to_buffer(std::vector<unsigned int> const& data,
                                     Mesh_Chunk& c, std::size_t element_offset,
                                     int base_vertex);

  Mesh_Chunk write_standard_data_to_mesh_buffers(
    Indexed_Mesh_Data const& data, Maybe_Owned<IMesh> mesh,
    std::vector<std::unique_ptr<IBuffer> > bufs,
    std::size_t vertex_element_offset, std::size_t element_array_offset
  );
  Mesh_Chunk write_standard_data_to_mesh_buffers(
    Ordered_Mesh_Data const& data, Maybe_Owned<IMesh> mesh,
    std::vector<std::unique_ptr<IBuffer> > bufs,
    std::size_t vertex_element_offset
  );

} }
