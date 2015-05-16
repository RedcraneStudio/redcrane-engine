/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <vector>
#include "../common/software_texture.h"
#include "../common/mesh.h"
namespace game
{
  struct Terrain
  {
    // Let's use a tile based system, for now. The integer represents altitude.
    std::vector< std::vector<int> > altitude;

    int w;
    int h;
  };

  Terrain make_flat_terrain(int alt, int w, int h);
  Terrain make_terrain_from_heightmap(Software_Texture const& tex,
                                      int add = -0xff / 2);

  Mesh_Data make_terrain_mesh(Terrain const& t, double scale_fac = .01,
                              double flat_fac = 1) noexcept;

  inline void make_terrain_mesh(Mesh& m, Terrain const& t, double s = .01,
                                double f = 1) noexcept
  {
    m.allocate_from(std::move(make_terrain_mesh(t, s, f)));
  }
}
