/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../../common/maybe_owned.hpp"
#include "../mesh_chunk.h"
#include "../mesh_data.h"
#include "../mesh.h"
#include "../../common/log.h"
namespace redc { namespace gfx
{
  // All offsets here are in elements, relative to the size of whatever element
  // is being used. These are not byte offsets!!

  void allocate_mesh_buffers(Indexed_Mesh_Data const& data, Mesh& mesh,
                             Upload_Hint up = Upload_Hint::Static,
                             Usage_Hint us = Usage_Hint::Draw) noexcept;
  void allocate_mesh_buffers(Ordered_Mesh_Data const& data, Mesh& mesh,
                             Upload_Hint up = Upload_Hint::Static,
                             Usage_Hint us = Usage_Hint::Draw) noexcept;

  void format_mesh_buffers(Mesh&) noexcept;

  void write_vertices_to_mesh(std::vector<Vertex> const& data,
                              Mesh_Chunk& chunk,
                              Maybe_Owned<Mesh> mesh,
                              unsigned int element_offset = 0) noexcept;

  void write_element_array_to_mesh(std::vector<unsigned int> const& data,
                                   Mesh_Chunk& c,
                                   Maybe_Owned<Mesh> mesh,
                                   unsigned int buf,
                                   unsigned int element_offset = 0,
                                   int base_vertex = 0) noexcept;

  Mesh_Chunk write_data_to_mesh(Indexed_Mesh_Data const& data,
                                Maybe_Owned<Mesh> mesh,
                                unsigned int vertice_element_offset = 0,
                                unsigned int elemnt_array_offset = 0) noexcept;

  Mesh_Chunk write_data_to_mesh(Ordered_Mesh_Data const& data,
                                Maybe_Owned<Mesh> mesh,
                                unsigned int vert_elemnt_off = 0) noexcept;
} }
