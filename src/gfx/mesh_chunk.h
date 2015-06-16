/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "mesh.h"
#include "../common/maybe_owned.hpp"
#include <boost/optional.hpp>
namespace game
{
  struct Mesh_Chunk
  {
    unsigned int start;
    unsigned int count;

    Primitive_Type type = Primitive_Type::Triangle;

    Maybe_Owned<Mesh> mesh;

    boost::optional<int> base_vertex;
  };

  namespace gfx
  {
    void render_chunk(Mesh_Chunk const&) noexcept;
  }
}
