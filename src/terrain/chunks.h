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
    // The dimensions in vertices / cells of this node's mesh / grid.
    // This should never be negative, but size_t makes me scared of underflow.
    // Set in initialize_vertices only for nodes that have initialized mesh.
    Vec<int> grid_size;

    // World space volume of this object.
    // Set in set_volumes.
    Volume<float> world_vol;

    // The area of the heightmap that we cover in its coordinate system.
    // Set for all nodes in set_volumes.
    Volume<int> uv_vol;

    // The root chunk should own it's mesh, the rest should point to it.
    Mesh_Chunk mesh_chunk;
  };

  using terrain_tree_t = Quadtree<Chunk>;

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

  namespace detail
  {
    /*!
     * \brief Returns the amount of vertices for a mesh that would contain a
     * grid of vertices x vertices for each node in levels [start_level, depth).
     */
    std::size_t mesh_vertices(std::size_t start_level, std::size_t depth,
                              std::size_t vertices) noexcept;
  }

  /*!
   * \brief Creates a quadtree with the proper levels and volumes.
   * for each depth level.
   */
  void set_volumes(terrain_tree_t& tr, Vec<float> world, Vec<int> uv) noexcept;

  /*!
   * \brief Initialize the vertices of the quadtree.
   *
   * The root node should own it's mesh.
   * \param tree The quadtree to initialize with vertices.
   * \param idriver The driver to use to allocate the mesh buffer.
   * \param level Depth level to start at.
   * \param vertices Grid size of each node. Note that each successive level
   * means one-forth of the actual area. The grid size will be used for every
   * node starting at level node.
   */
  void initialize_vertices(terrain_tree_t& tree, gfx::IDriver& idriver,
                           std::size_t level, std::size_t vertices) noexcept;
  void render_level(terrain_tree_t& tree, gfx::IDriver& idriver,
                    std::size_t level) noexcept;
} }
