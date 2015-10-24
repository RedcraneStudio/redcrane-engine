/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "chunks.h"
#include "../gfx/mesh_data.h"
#include "../gfx/support/allocate.h"
#include "../gfx/support/write_data_to_mesh.h"
namespace game { namespace terrain
{
  //! Wat
  int distance_from_water(Vec<int> pos, gen::Grid_Map const& map,
                          int max_dist) noexcept
  {
    int found_at = 0;

    // We found a distance, get outta here.
    for(int i = 1; i <= max_dist && !found_at; ++i)
    {
      if(pos.x - i < 0 || pos.x + i > map.extents.x ||
         pos.y - i < 0 || pos.y + i > map.extents.y) continue;

      // pos needs to go from {pos.x - i, pos.y - i} to {pos.x + i, pos.y + i}
      // and back around the other way for each distance level i.

      // # = path
      // # # # # #
      // # - - - #
      // # - - - #
      // # - - - #
      // # # # # #
      // i = 2 in the case above

      // Simplification of (i * 2) + 1 + (i * 2) + (i * 2) + (i * 2) - 1
      // (i * 2) + 1: The amount of pixels on top
      // (i * 2) + (i * 2): On both sides
      // (i * 2) - 1: On the bottom
      for(int j = 0; j < 4 * (i * 2); ++j)
      {
        // At the top
        if(j < (i * 2) + 1)
        {
          // Start here .
          // .----------^
          // # # # # #
          // # - - - #
          // # - - - #
          // # - - - #
          // # # # # #
          auto off = j % ((i * 2) + 1);
          if(map.at({pos.x - i + off, pos.y - i}).type == gen::Cell_Type::Water)
          {
            found_at = i;
          }
        }
        // At the right
        else if(j < 2 * (i * 2) + 1)
        {
          // Start here -.
          //             |
          // # # # # #   |
          // # - - - # <-^
          // # - - - #
          // # - - - #
          // # # # # #
          auto off = j % (2 * (i * 2) + 1);
          if(map.at({pos.x + i, pos.y - i + off + 1}).type == gen::Cell_Type::Water)
          {
            found_at = i;
          }
        }
        // Search the left
        else if(j < 3 * (i * 2) + 1)
        {
          // Start here .
          // .----------^
          // |   # # # # #
          // ^-> # - - - #
          //     # - - - #
          //     # - - - #
          //     # # # # #
          auto off = j % (3 * (i * 2) + 1);
          if(map.at({pos.x - i, pos.y - i + off + 1}).type == gen::Cell_Type::Water)
          {
            found_at = i;
          }
        }
        // Search on the bottom
        else
        {
          // Start here .
          // .----------^
          // |   # # # # #
          // |   # - - - #
          // |   # - - - #
          // |   # - - - #
          // |   # # # # #
          // ^----^    ^
          // _____|    |_____
          // | And go to... |
          // ----------------
          auto off = j % (4 * (i * 2));
          if(map.at({pos.x - i + off + 1, pos.y + i}).type == gen::Cell_Type::Water)
          {
            found_at = i;
          }
        }
      }
    }

    return found_at;
  }

  // Done once, hopefully not that awful in terms of performance
  Heightmap make_heightmap(gen::Grid_Map const& map) noexcept
  {
    Heightmap ret;
    ret.allocate(map.extents);

    for(int i = 0; i < map.extents.x * map.extents.y; ++i)
    {
      float value = 0.0f;

      switch(map.values[i].type)
      {
      case gen::Cell_Type::Water:
        value = 0.0f;
        break;
      case gen::Cell_Type::Land:
        {
          // Get the position in cartesian coordinates.
          auto pos = Vec<int>{i % map.extents.x, i / map.extents.x};
          // Get the distance of the current point from water.
          auto distance = distance_from_water(pos, map, 5);
          // If it returns zero that means we are not close at all.
          if(!distance) value = 1.0f;
          // Otherwise the shorter the distance the less height we give the
          // location on the heightmap.
          else value = 1.0f - distance / 5.0f;
          break;
        }
      }

      ret.values[i] = value;
    }

    return ret;
  }

  void set_volumes(terrain_tree_t& tree, Vec<int> extents) noexcept
  {
    // The volume of the root node is the entire heightmap.
    tree.node_at_depth(0,0).val.vol = vol_from_extents<int>(extents);

    // Don't start at the first node, since we dealt with that one.
    for(auto iter = tree.level_begin(1); iter != tree.end(); ++iter)
    {
      // Find the offset of the current iter from the beginning node of the
      // current depth level.
      auto offset_from_level = iter - tree.level_begin(iter->depth());

      // Children per node is definitely equal to four.
      // Results in something like this. Corner =
      // _____
      // |0|1|
      // -----
      // |2|3|
      // -----
      auto corner = offset_from_level % tree.children_per_node();

      // This is the index from the beginning of the previous depth level
      // representing the parent.
      auto parent_off = offset_from_level / tree.children_per_node();

      // This is the parent node of the current node.
      auto& parent_node = tree.node_at_depth(iter->depth() - 1, parent_off);

      // Get the proper volume for this current node.
      iter->val.vol = vol_quad(parent_node.val.vol, corner);
    }
  }
  void set_physical_size(terrain_tree_t& tree, Vec<float> physical_size)
  {
    // Set the physical size of each and every node in the quadtree.

    // We may be able to make this faster by combining this function / loop in
    // initialize_vertices and not going through the entire tree. We could only
    // iterate through [begin(), level_end(start_level-1)) and then add the
    // same code to the mesh-gen loop which would cover the rest of the depth
    // levels. It would be a simple form of loop unrolling I guess. For now
    // this is the simplest implementation.
    for(auto iter = tree.level_begin(0); iter != tree.end(); ++iter)
    {
      iter->val.physical_size =
        {physical_size.x / (float) std::pow(2, iter->depth()),
         physical_size.y / (float) std::pow(2, iter->depth())};
    }
  }
  void initialize_vertices(terrain_tree_t& tree, gfx::IDriver& idriver,
                           std::size_t level, std::size_t vertices) noexcept
  {
    // Create a mesh and give the root node ownership.
    auto mesh = Maybe_Owned<Mesh>{idriver.make_mesh_repr()};

    // Figure out the final size of the mesh.
    auto final_verts =detail::mesh_vertices(level, tree.get_depth(), vertices);
    gfx::allocate_standard_mesh_buffers(final_verts, *mesh, Usage_Hint::Draw,
                                        Upload_Hint::Static);
    gfx::format_mesh_buffers(*mesh);

    // Since we know mesh is a maybe owned, we can guarantee that moving from
    // it will leave it in a consistent state of being unowned.
    tree.node_at_index(0).val.mesh_chunk.mesh = std::move(mesh);

    // Start at the first level to generate mesh for and go to the end of the
    // tree.
    for(auto iter = tree.level_begin(level); iter != tree.end(); ++iter)
    {
      // Initialize the grid size.
      iter->val.grid_size = {(int) vertices, (int) vertices};

      // What is the size of each cell? Physical size (of the current node) /
      // the amount of vertices in the grid (that fully contains the node).
      Vec<float> cell_size;
      cell_size.x = iter->val.physical_size.x / iter->val.grid_size.x;
      cell_size.y = iter->val.physical_size.y / iter->val.grid_size.y;

      // For each level there is some mesh data.
      Ordered_Mesh_Data mesh_data;
      // For each grid cell, there are 2 triangles of three vertices.
      for(int i = 0; i < (int) vertices * (int) vertices; ++i)
      {

        // TODO: UVs should match the heightmap texture.
        // Normals for now should always be up
        // TODO: Make sure we need CCW winding order.
        // TODO: Add root data to the quadtree.

        int x = i % vertices;
        int y = i / vertices;

        Vertex v0;
        v0.position = glm::vec3((float) x, 0.0f, (float) y);
        v0.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        //v0.uv = glm::vec2(0.0f, 0.0f);
        mesh_data.vertices.push_back(std::move(v0));

        Vertex v1;
        v1.position = glm::vec3((float) x + cell_size.x, 0.0f, (float) y);
        v1.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        //v0.uv = glm::vec2(0.0f, 0.0f);
        mesh_data.vertices.push_back(std::move(v1));

        Vertex v2;
        v2.position = glm::vec3((float) x + cell_size.x, 0.0f,
                                (float) y + cell_size.y);
        v2.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        //v0.uv = glm::vec2(0.0f, 0.0f);
        mesh_data.vertices.push_back(std::move(v2));

        mesh_data.vertices.push_back(v0);
        mesh_data.vertices.push_back(v2);

        Vertex v3;
        v3.position = glm::vec3((float) x, 0.0f, (float) y + cell_size.y);
        v3.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        //v0.uv = glm::vec2(0.0f, 0.0f);
        mesh_data.vertices.push_back(std::move(v3));
      }

      // Append this node's mesh data to the end of the mesh.

      // Just reference the mesh, the root is going to own it.
      iter->val.mesh_chunk = gfx::write_data_to_mesh(mesh_data, ref_mo(mesh),0);
    }
  }
} }
