/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../common/aabb.h"
#include "../gfx/object.h"
namespace game
{
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
