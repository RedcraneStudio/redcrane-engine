/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <vector>
#include "../common/aabb.h"
#include "../gfx/material.h"
#include "../common/mesh.h"
#include "../gfx/object.h"
#include "../gfx/idriver.h"
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

  Mesh_Data make_terrain_mesh(Terrain const& t, double scale_fac = .01,
                              double flat_fac = 1) noexcept;

  inline void make_terrain_mesh(Mesh& m, Terrain const& t, double s = .01,
                                double f = 1) noexcept
  {
    m.prepare(std::move(make_terrain_mesh(t, s, f)));
  }

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

    /*!
     * This function must always return the same mesh and material (points
     * to the same object).
     */
    virtual gfx::Object make_obj() const noexcept = 0;
  };

  struct Structure_Instance
  {
    Structure_Instance(IStructure&, Orient) noexcept;
    Structure_Instance(Structure_Instance const&) noexcept;
    Structure_Instance& operator=(Structure_Instance const& i) noexcept;

    ~Structure_Instance() noexcept = default;

    const IStructure* structure_type;
    gfx::Object obj;
    Orient orientation;
  };
}
