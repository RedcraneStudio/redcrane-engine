/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <vector>
namespace strat
{
  struct Terrain
  {
    // Let's use a tile based system, for now. The integer represents altitude.
    std::vector< std::vector<int> > altitude;
  };

  Terrain make_flat_terrain(int alt, int w, int h);

  enum class Orient
  {
    N,
    E,
    S,
    W
  };

  struct AABB
  {
    int depth;
    int height;
    int width;
  };

  struct IStructure
  {
    virtual ~IStructure() noexcept {}

    virtual AABB aabb() const noexcept = 0;

    virtual Orient orientation() const noexcept = 0;

    virtual int style_id() const noexcept = 0;
    virtual int mesh_id() const noexcept = 0;
  };
}
