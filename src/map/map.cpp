/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "map.h"
#include "../common/grid_iterator.h"
namespace strat
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
  Mesh make_terrain_mesh(Terrain const& t, double scale_fac,
                         double flat_fac) noexcept
  {
    std::vector<glm::vec3> quad_vertices;
    std::vector<glm::vec2> tex_coords;
    std::vector<unsigned int> faces { 1, 3, 2, 0, 1, 2 };

    quad_vertices.emplace_back(0.0, 0.0, 1.0);
    quad_vertices.emplace_back(0.0, 0.0, 0.0);
    quad_vertices.emplace_back(1.0, 0.0, 1.0);
    quad_vertices.emplace_back(1.0, 0.0, 0.0);

    tex_coords.emplace_back(0.0, 1.0);
    tex_coords.emplace_back(0.0, 0.0);
    tex_coords.emplace_back(1.0, 1.0);
    tex_coords.emplace_back(1.0, 0.0);

    auto mesh = Mesh{};

    // Add some amount of rectangles, then adjust their height.
    // Doesn't work when t.h or t.w == 1 or 0
    auto iteration = 0;
    for(int i = 0; i < t.h - 1; ++i)
    {
      for(int j = 0; j < t.w - 1; ++j)
      {
        for(auto vertex : quad_vertices)
        {
          // Insert the vertex adjusting it based on our current row / col.
          // Z is based on terrain height value.
          // Scale it a bit, also.
          mesh.vertices.emplace_back((vertex.x + j) * flat_fac,
                                     t.altitude[i][j] * scale_fac,
                                     (vertex.z + i) * flat_fac);
        }
        for(auto uv : tex_coords)
        {
          // No need to adjust texture coordinates, the texture will repeat
          // along every quad.
          mesh.tex_coords.push_back(uv);
        }
        for(auto face : faces)
        {
          // Adjust the face by our current iteration.
          mesh.faces.push_back(face + quad_vertices.size() * iteration);
        }

        ++iteration;
      }
    }

    return mesh;
  }
  Structure_Instance::
  Structure_Instance(IStructure* s, Orient o) noexcept
    : structure_type(s), orientation(o) {}
  Structure_Instance::Structure_Instance(Structure_Instance const& s) noexcept
    : structure_type(s.structure_type), orientation(s.orientation) {}
}
