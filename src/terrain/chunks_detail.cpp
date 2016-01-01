/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "chunks.h"
namespace redc { namespace terrain { namespace detail
{
  std::size_t mesh_vertices(std::size_t start_level, std::size_t depth,
                            std::size_t vertices) noexcept
  {
    // This is a accumulator, so we won't mess around with any subtraction
    // which is why it's a size_t.
    std::size_t final_verts = 0;

    for(std::size_t level_i = start_level; level_i < depth; ++level_i)
    {
      // For each level that we are allocating, we have:
      // vertices * vertices * 4^level_i grid cells with 6 vertices per
      // grid cell.

      final_verts += vertices * vertices * std::pow(4, level_i) * 6;
    }

    return final_verts;
  }
} } }
