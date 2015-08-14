/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <vector>
#include "../common/color.h"
#include "../common/aabb.h"
#include "../gfx/support/software_texture.h"
#include "../gfx/mesh_data.h"
#include "../gfx/mesh_chunk.h"
namespace game { namespace strat
{
  struct Heightmap
  {
    void allocate(Vec<int> extents) noexcept;
    ~Heightmap() noexcept;

    int16_t* vals = nullptr;
    Vec<int> extents;
    bool allocated;
  };

  struct Terrain_Chunk
  {
    AABB aabb;

    // Represent indices into the Ordered Mesh Data stored in the Terrain Mesh.
    unsigned int start = 0;
    unsigned int count = 0;
  };

  struct Terrain_Mesh
  {
    Indexed_Mesh_Data mesh;

    std::vector<Terrain_Chunk> chunks;
  };

  Heightmap make_flat_heightmap(int16_t alt, int w, int h);
  Heightmap make_heightmap_from_image(Software_Texture const& tex,
                                      int add = -0xff / 2);

  // Extents are given in vertices.
  Terrain_Mesh make_terrain_mesh(Heightmap const& heights,
                                 Vec<int> chunk_extents = {20, 20},
                                 double y_scale = .01,
                                 double flat_scale = 1,
                                 bool gen_aabbs = false) noexcept;

} }
