/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "map.h"
#include "../common/grid_iterator.h"
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
  Terrain_Mesh make_terrain_mesh(Heightmap const& heights,
                                 Vec<float> chunk_extents, double y_scale,
                                 double flat_scale) noexcept
  {
    auto mesh = Terrain_Mesh{};
    mesh.mesh = make_maybe_owned<Mesh_Data>();

    // Add a vertex with a given height for each altitude given.
    for(int i = 0; i < heights.extents.y; ++i)
    {
      for(int j = 0; j < heights.extents.x; ++j)
      {
        Vertex v;

        v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        v.uv = glm::vec2(0.0f, 0.0f);
        v.position.x = j * flat_scale;
        v.position.y = heights.vals[i * heights.extents.x + j] * y_scale;
        v.position.z = i * flat_scale;

        mesh.mesh->vertices.push_back(v);
      }
    }

    // Add faces.
    std::array<unsigned int, 6> face_indices{ 0, 3, 2, 0, 1, 2 };

    // For the amount of rectangles.
    auto e = heights.extents;

    int num_rects = (e.x - 1) * (e.y - 1);
    for(int i = 0; i < num_rects; ++i)
    {
      int x = i % e.x;
      int y = i / e.y;

      if(x == e.x - 1 || y == e.y - 1)
      {
        // If we are at the last vertex, just bail out. Trying to make a face
        // here will make one across the mesh which would be terrible.
        continue;
      }

      mesh.mesh->elements.push_back(x + 0 + y * e.x);
      mesh.mesh->elements.push_back(x + 1 + (y + 1) * e.x);
      mesh.mesh->elements.push_back(x + 1 + y * e.x);
      mesh.mesh->elements.push_back(x + 0 + y * e.x);
      mesh.mesh->elements.push_back(x + 0 + (y + 1) * e.x);
      mesh.mesh->elements.push_back(x + 1 + (y + 1) * e.x);
    }

    // Set the uv coordinates over the whole mesh.
    for(int i = 0; i < e.y; ++i)
    {
      for(int j = 0; j < e.x; ++j)
      {
        mesh.mesh->vertices[i * e.x + j].uv =
          glm::vec2((float) j / e.x, (float) i / e.y);
      }
    }

    // Change the mesh defaults, of course the user can just switch these as
    // soon as they get ahold of the mesh.
    mesh.mesh->usage_hint = Usage_Hint::Draw;
    mesh.mesh->upload_hint = Upload_Hint::Static;
    mesh.mesh->primitive = Primitive_Type::Triangle;

    return mesh;
  }
}
