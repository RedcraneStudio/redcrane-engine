/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "terrain.h"
#include "../common/grid_iterator.h"

#include "../common/debugging.h"
namespace game { namespace strat
{
  void Heightmap::allocate(Vec<int> e) noexcept
  {
    vals = new int16_t[area(e)];
    extents = e;
    allocated = true;
  }
  Heightmap::~Heightmap() noexcept
  {
    delete[] vals;
  }

  Heightmap make_flat_heightmap(int16_t alt, int w, int h)
  {
    Heightmap heights{};

    heights.allocate({w, h});

    for(int i = 0; i < h; ++i)
    {
      for(int j = 0; j < w; ++j)
      {
        heights.vals[i * w + j] = alt;
      }
    }

    return heights;
  }
  Heightmap make_heightmap_from_image(Software_Texture const& tex, int add)
  {
    Heightmap ret{};

    ret.allocate(tex.allocated_extents());

    for(int i = 0; i < ret.extents.y; ++i)
    {
      for(int j = 0; j < ret.extents.x; ++j)
      {
        // Just use the red color.
        int col = *tex.get_pt({i, j}) * 0xff;
        ret.vals[i * ret.extents.x + j] = col + add;
      }
    }
    return ret;
  }
  Terrain_Mesh make_terrain_mesh(Heightmap const& height,
                                 Vec<int> chunk_extents,
                                 double y_scale,
                                 double flat_scale,
                                 bool gen_aabbs) noexcept
  {
    auto tmesh = Terrain_Mesh{};

    // Add a vertex with a given height for each altitude given.
    for(int i = 0; i < height.extents.y; ++i)
    {
      for(int j = 0; j < height.extents.x; ++j)
      {
        Vertex v;

        v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        v.uv = glm::vec2(0.0f, 0.0f);
        v.position.x = j * flat_scale;
        v.position.y = height.vals[i * height.extents.x + j] * y_scale;
        v.position.z = i * flat_scale;

        tmesh.mesh.vertices.push_back(v);
      }
    }

    // For the amount of rectangles.
    auto e = height.extents;

    // Separate into segments.
    int chunks_x = e.x / chunk_extents.x;
    // Add a segment if the amount we just calculated would be short.
    chunks_x += chunks_x * chunk_extents.x != e.x ? 1 : 0;

    int chunks_y = e.y / chunk_extents.y;
    chunks_y += chunks_y * chunk_extents.y != e.y ? 1 : 0;

    unsigned int cur_offset = 0;

    for(int i = 0; i < chunks_y; ++i)
    {
      for(int j = 0; j < chunks_x; ++j)
      {
        Terrain_Chunk chunk;

        chunk.start = cur_offset;
        chunk.count = 0;

        // Get the minimum components from every point.
        for(int vertex_i = 0; vertex_i < chunk_extents.y; ++vertex_i)
        {
          for(int vertex_j = 0; vertex_j < chunk_extents.x; ++vertex_j)
          {
            int y = i * chunk_extents.y + vertex_i;
            int x = j * chunk_extents.x + vertex_j;

            // If we can't be here. This will only happen with the rightmost /
            // bottommost chunk.
            if(e.y <= y || e.x <= x) continue;

            // We basically need to access a small sub volume of the mesh.
            auto vertex = tmesh.mesh.vertices[y * e.x + x];

            chunk.aabb.min.x  = std::min(chunk.aabb.min.x,  vertex.position.x);
            chunk.aabb.min.y  = std::min(chunk.aabb.min.y,  vertex.position.y);
            chunk.aabb.min.z  = std::min(chunk.aabb.min.z,  vertex.position.z);

            chunk.aabb.width  = std::max(chunk.aabb.width,  vertex.position.x);
            chunk.aabb.height = std::max(chunk.aabb.height, vertex.position.y);
            chunk.aabb.depth  = std::max(chunk.aabb.depth,  vertex.position.z);

            if(x == e.x - 1 || y == e.y - 1)
            {
              // If we are at the last vertex, just bail out. Trying to make a face
              // here will make one across the mesh which would be terrible.
              continue;
            }

            // Add the faces while we are at it.
            tmesh.mesh.elements.push_back(x + 0 + y * e.x);
            tmesh.mesh.elements.push_back(x + 1 + (y + 1) * e.x);
            tmesh.mesh.elements.push_back(x + 1 + y * e.x);
            tmesh.mesh.elements.push_back(x + 0 + y * e.x);
            tmesh.mesh.elements.push_back(x + 0 + (y + 1) * e.x);
            tmesh.mesh.elements.push_back(x + 1 + (y + 1) * e.x);

            chunk.count += 6;
          }
        }

        cur_offset += chunk.count;

        tmesh.chunks.push_back(chunk);
      }
    }

    // Set the uv coordinates over the whole mesh.
    for(int i = 0; i < e.y; ++i)
    {
      for(int j = 0; j < e.x; ++j)
      {
        tmesh.mesh.vertices[i * e.x + j].uv =
          glm::vec2((float) j / e.x, (float) i / e.y);
      }
    }

    return tmesh;
  }
} }
