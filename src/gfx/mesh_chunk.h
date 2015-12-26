/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "mesh.h"
#include "../common/maybe_owned.hpp"
#include <boost/optional.hpp>
namespace game { namespace gfx
{
  struct Mesh_Chunk
  {
    unsigned int start;
    unsigned int count;

    Primitive_Type type = Primitive_Type::Triangle;

    Maybe_Owned<Mesh> mesh;

    boost::optional<int> base_vertex;
  };

  /*!
   * \brief Copies a mesh chunk so the copy's mesh points to the original.
   */
  Mesh_Chunk copy_mesh_chunk_share_mesh(Mesh_Chunk const&) noexcept;
  /*!
   * \brief Copies a mesh chunk so the new copy owns the mesh.
   */
  Mesh_Chunk copy_mesh_chunk_move_mesh(Mesh_Chunk&) noexcept;

  void render_chunk(Mesh_Chunk const&) noexcept;
} }
