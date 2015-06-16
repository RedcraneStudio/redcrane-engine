/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "map.h"
#include "../common/grid_iterator.h"

#include "../common/debugging.h"
namespace game
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
        auto col = tex.get_pt({i, j});
        ret.vals[i * ret.extents.x + j] = col.r + add;
      }
    }
    return ret;
  }
  Terrain_Mesh make_terrain_mesh(Heightmap const& height,
                                 Vec<int> chunk_extents, double y_scale,
                                 double flat_scale, bool gen_aabbs) noexcept
  {
    auto mesh = Terrain_Mesh{};

    // For the amount of rectangles.
    auto e = height.extents;

    // Separate into segments.
    int chunks_x = e.x / chunk_extents.x;
    // Add a segment if the amount we just calculated would be short.
    chunks_x += chunks_x * chunk_extents.x != e.x ? 1 : 0;

    int chunks_y = e.y / chunk_extents.y;
    chunks_y += chunks_y * chunk_extents.y != e.y ? 1 : 0;

    unsigned int cur_offset = 0;

    auto size = chunks_y * chunks_x * chunk_extents.y * chunk_extents.x * 6;
    mesh.mesh.vertices.resize(size);

    for(int i = 0; i < chunks_y; ++i)
    {
      for(int j = 0; j < chunks_x; ++j)
      {
        Terrain_Chunk chunk;
        chunk.start = cur_offset;

        // For each first vertex in a face in the chunk
        for(int vertex_i = 0; vertex_i < chunk_extents.y; ++vertex_i)
        {
          for(int vertex_j = 0; vertex_j < chunk_extents.x; ++vertex_j)
          {
            int y = i * chunk_extents.y + vertex_i;
            int x = j * chunk_extents.x + vertex_j;

            for(int vert_i = 0; vert_i < 6; vert_i++)
            {
              auto& v = mesh.mesh.vertices[(y * e.x + x) * 6 + vert_i];

              // Generate a normal using the cross product.
              v.normal = glm::vec3(0.0f, 1.0f, 0.0f);

              v.uv = glm::vec2(x / (float) e.x, y / (float) e.y);

              int add_x = vert_i == 1 || vert_i == 2 || vert_i == 5 ? 1 : 0;
              int add_y = vert_i == 1 || vert_i == 4 || vert_i == 5 ? 1 : 0;

              // If we can't be here. This will only happen with the
              // rightmost / bottommost chunk.
              if(e.y <= (y + add_y) || e.x <= x + (add_x)) continue;

              // The x-z position is generated.
              v.position.x = (x + add_x) * flat_scale;
              v.position.z = (y + add_y) * flat_scale;

              // The height is from the heightmap.
              v.position.y = height.vals[(y + add_y) * e.x +
                                         (x + add_x)] * y_scale;

              ++cur_offset;

              if(gen_aabbs)
              {
                chunk.aabb.min.x  = std::min(chunk.aabb.min.x,  v.position.x);
                chunk.aabb.min.y  = std::min(chunk.aabb.min.y,  v.position.y);
                chunk.aabb.min.z  = std::min(chunk.aabb.min.z,  v.position.z);

                chunk.aabb.width  = std::max(chunk.aabb.width,  v.position.x);
                chunk.aabb.height = std::max(chunk.aabb.height, v.position.y);
                chunk.aabb.depth  = std::max(chunk.aabb.depth,  v.position.z);
              }
            }
          }
        }

        chunk.count = cur_offset - chunk.start;

        mesh.chunks.push_back(chunk);
      }
    }

    mesh.mesh.vertices.resize(cur_offset);

    GAME_ASSERT(cur_offset == mesh.mesh.vertices.size());

    return mesh;
  }
}
