/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <vector>
#include "../common/color.h"
#include "../common/aabb.h"
#include "../common/software_texture.h"
#include "../common/mesh.h"
#include "../common/software_mesh.h"
namespace game
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
    Mesh_Chunk mesh;
  };

  struct Terrain_Mesh
  {
    Maybe_Owned<Mesh_Data> mesh;

    std::vector<Terrain_Chunk> chunks;
  };

  Heightmap make_flat_heightmap(int16_t alt, int w, int h);
  Heightmap make_heightmap_from_image(Software_Texture const& tex,
                                        int add = -0xff / 2);

  // Extents are given in vertices.
  Terrain_Mesh make_terrain_mesh(Heightmap const& heights,
                                 Vec<int> chunk_extents = {20, 20},
                                 double y_scale = .01,
                                 double flat_scale = 1) noexcept;

  // We require a software mesh because only with a software mesh can we access
  // the mesh_data after moving it into the mesh. This is the only expected
  // thing to do so that our maybe owned mesh data in the terrain mesh can
  // *point* to the data in the software mesh.
  inline Terrain_Mesh make_terrain_mesh(Software_Mesh& m,
                                        Heightmap const& heights,
                                        Vec<unsigned int> ce = {20,20},
                                        double y_scale = .01,
                                        double flat_scale = 1) noexcept
  {
    auto terrain_mesh = make_terrain_mesh(heights, ce, y_scale, flat_scale);
    m.allocate_from(std::move(*terrain_mesh.mesh));
    terrain_mesh.mesh.set_pointer(&m.mesh_data());
    return terrain_mesh;
  }
}
