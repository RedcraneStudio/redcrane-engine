/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "map.h"
#include "../common/grid_iterator.h"
namespace game
{
  Terrain make_flat_terrain(int alt, int w, int h)
  {
    auto terrain = Terrain{};
    for(int i = 0; i < h; ++i)
    {
      terrain.altitude.emplace_back();
      for(int j = 0; j < w; ++j)
      {
        terrain.altitude.back().push_back(alt);
      }
    }

    terrain.w = w;
    terrain.h = h;

    return terrain;
  }
  Terrain make_terrain_from_heightmap(Software_Texture const& tex, int add)
  {
    auto ret = Terrain{};
    ret.w = tex.allocated_extents().x;
    ret.h = tex.allocated_extents().y;

    for(int i = 0; i < ret.h; ++i)
    {
      ret.altitude.emplace_back();
      for(int j = 0; j < ret.w; ++j)
      {
        // Just use the red color.
        auto col = tex.get_pt({i, j});
        ret.altitude.back().push_back((int) col.r + add);
      }
    }
    return ret;
  }
  Mesh_Data make_terrain_mesh(Terrain const& t, double scale_fac,
                              double flat_fac) noexcept
  {
    auto mesh = Mesh_Data{};

    // Add a vertex with a given height for each altitude given.
    for(int i = 0; i < t.h; ++i)
    {
      for(int j = 0; j < t.w; ++j)
      {
        Vertex v;

        v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        v.uv = glm::vec2(0.0f, 0.0f);
        v.position.x = j * flat_fac;
        v.position.y = t.altitude[i][j] * scale_fac;
        v.position.z = i * flat_fac;

        mesh.vertices.push_back(v);
      }
    }

    // Add faces.
    std::array<unsigned int, 6> face_indices{ 0, 3, 2, 0, 1, 2 };

    // For the amount of rectangles.
    int num_rects = (t.w - 1) * (t.h - 1);
    for(int i = 0; i < num_rects; ++i)
    {
      int x = i % t.w;
      int y = i / t.w;

      if(x == t.w - 1 || y == t.h - 1)
      {
        // If we are at the last vertex, just bail out. Trying to make a face
        // here will make one across the mesh which would be terrible.
        continue;
      }

      mesh.elements.push_back(x + 0 + y * t.w);
      mesh.elements.push_back(x + 1 + (y + 1) * t.w);
      mesh.elements.push_back(x + 1 + y * t.w);
      mesh.elements.push_back(x + 0 + y * t.w);
      mesh.elements.push_back(x + 0 + (y + 1) * t.w);
      mesh.elements.push_back(x + 1 + (y + 1) * t.w);
    }

    // Set the uv coordinates over the whole mesh.
    for(int i = 0; i < t.h; ++i)
    {
      for(int j = 0; j < t.w; ++j)
      {
        mesh.vertices[i * t.w + j].uv = glm::vec2((float) j / t.w,
                                                  (float) i / t.h);
      }
    }

    // Change the mesh defaults, of course the user can just switch these as
    // soon as they get ahold of the mesh.
    mesh.usage_hint = Usage_Hint::Draw;
    mesh.upload_hint = Upload_Hint::Static;
    mesh.primitive = Primitive_Type::Triangle;

    return mesh;
  }
}
