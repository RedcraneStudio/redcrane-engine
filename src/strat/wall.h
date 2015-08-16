/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <cstdint>
#include <vector>
#include <string>

#include "../common/vec.h"

#include "../gfx/idriver.h"

namespace game { namespace strat
{
  struct Wall_Type
  {
    // Id for now will be enough to differenciate between different wall types.
    uint8_t id;

    // Negative for no unit size.
    float unit_size;
  };

  struct Wall_Segment
  {
    Wall_Type type;

    // Indices into the Wall::points vector.
    std::pair<std::size_t, std::size_t> indices;
    // Amount of layers of the wall at a specific location
    int8_t layers;
  };

  struct Wall
  {
    std::vector<Vec<float> > points;

    // Segments link the points together.
    std::vector<Wall_Segment> segments;
  };

  struct Pending_Wall
  {
    Wall_Type type;

    // Starting position, once we have an end position we can just add the
    // points to the wall.
    Vec<float> pos;
  };

  // Takes a pending wall and an endpoint and adds it to an existing wall.
  void end_wall(Pending_Wall const& pending_wall, Wall& existing,
                Vec<float> map_position) noexcept;

  // Snaps an endpoint onto some axis aligned with the pending wall's initial
  // point.
  Vec<float> pending_wall_end_pt(Pending_Wall const& pending_wall,
                                 Vec<float> const& map_pt) noexcept;

  // TODO: Move all the render_* somewhere other than scattering them across
  // stratlib
  struct Structure;
  void render_wall(gfx::IDriver&, Wall const&, Structure const&) noexcept;
} }
