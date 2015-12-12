/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../common/optional.h"

#include "../gfx/idriver.h"
#include "../gfx/mesh_chunk.h"
#include "../gfx/mesh_data.h"
namespace game { namespace gfx
{
  struct Mesh_Load_Params
  {
    std::string filename;

    bool retain_mesh = false;
  };

  struct Mesh_Result
  {
    Mesh_Chunk chunk;
    optional<Indexed_Mesh_Data> data;
  };

  Mesh_Result load_mesh(IDriver&, Mesh_Load_Params const&) noexcept;
} }
