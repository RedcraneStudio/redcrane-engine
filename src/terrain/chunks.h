/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once

#include <memory>
#include <vector>

#include "../common/tree.h"
#include "../common/volume.h"

#include "../gen/terrain.h"

#include "../gfx/idriver.h"
#include "../gfx/mesh.h"
#include "../gfx/mesh_chunk.h"
namespace game { namespace terrain
{
  using Heightmap = Value_Map<float>;
  Heightmap make_heightmap(gen::Grid_Map const&) noexcept;

  struct Chunk
  {
    // In local space? world space? I don't believe we should apply any scaling
    // so this size would be the same both ways.
    Vec<float> physical_size;

    // The amount of vertices in the mesh grid.
    Vec<std::size_t> mesh_size;

    // The area of the heightmap that we cover in its coordinate system.
    Volume<int> vol;

    // The root chunk should own it's mesh, the rest should point to it.
    Mesh_Chunk mesh_chunk;
  };

  // We should make sure we remember that some levels could simply lack
  // generated vertices (such as the root note) because rendering at that low
  // quality could be useless.

  // There are a couple ways we can approach this: 1) Use the heightmap (which
  // is presumably static) to pre-generate the levels with a y value from the
  // heightmap. This will avoid the need to sample the texture in the vertex
  // shader. 2) Generate vertices independent from the heightmap and sample
  // the height from the texture in the vertex shader.

  // I'm leaning on the first option because it's not impossible to update the
  // quadtree should the heightmap change (find the largest containing area and
  // work down to each child.)

  // Each level of this quadtree is a more detailed mesh containing one-fourth
  // of the area of it's parent. (Each level has the same amount of vertices.

  /*!
   * \brief Creates a quadtree with the proper levels and volumes.
   * for each depth level.
   */
  void set_levels_volumes(Quadtree<Chunk>& tree, Vec<int> extents,
                          std::size_t levels) noexcept;
} }
