/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <vector>
#include "../common/aabb.h"
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

  struct IStructure
  {
    virtual ~IStructure() noexcept {}

    virtual AABB aabb() const noexcept = 0;

    virtual int style_id() const noexcept = 0;
    virtual int mesh_id() const noexcept = 0;
  };

  struct Structure_Instance
  {
    Structure_Instance(IStructure*, Orient) noexcept;
    Structure_Instance(Structure_Instance const&) noexcept;

    ~Structure_Instance() noexcept = default;

    const IStructure* structure_type;
    const Orient orientation;
  };
}
