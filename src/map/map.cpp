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
  Mesh_Data make_terrain_mesh(Terrain const& t, double scale_fac,
                              double flat_fac) noexcept
  {
    std::array<Vertex, 4> vertices;
    std::vector<unsigned int> faces { 1, 3, 2, 0, 1, 2 };

    vertices[0].position = {0.0, 0.0, 1.0};
    vertices[1].position = {0.0, 0.0, 0.0};
    vertices[2].position = {1.0, 0.0, 1.0};
    vertices[3].position = {1.0, 0.0, 0.0};

    vertices[0].uv = {0.0, 1.0};
    vertices[1].uv = {0.0, 0.0};
    vertices[2].uv = {1.0, 1.0};
    vertices[3].uv = {1.0, 0.0};

    auto mesh = Mesh_Data{};

    // Add some amount of rectangles, then adjust their height.
    // Doesn't work when t.h or t.w == 1 or 0
    auto iteration = 0;
    for(int i = 0; i < t.h - 1; ++i)
    {
      for(int j = 0; j < t.w - 1; ++j)
      {
        std::array<Vertex, 4> mod_verts = vertices;

        for(auto& vertex : mod_verts)
        {
          // Scale the width and depth by flat_fac and the height by scale_fac.
          // We also translate the squares based on their position on the grid.
          vertex.position.x = (vertex.position.x + j) * flat_fac;
          vertex.position.y = t.altitude[i][j] * scale_fac;
          vertex.position.z = (vertex.position.z + i) * flat_fac;
        }

        using std::end;
        // Insert the vertices.
        mesh.vertices.insert(end(mesh.vertices), begin(mod_verts),
                             end(mod_verts));

        for(auto face : faces)
        {
          // Adjust the face by our current iteration.
          mesh.elements.push_back(face + vertices.size() * iteration);
        }

        ++iteration;
      }
    }

    return mesh;
  }
  Structure_Instance::
  Structure_Instance(IStructure& s, Orient o) noexcept
    : structure_type(&s),
      obj(structure_type->make_obj()),
      orientation(o) {}
  Structure_Instance::Structure_Instance(Structure_Instance const& s) noexcept
    : structure_type(s.structure_type),
      obj(share_object_keep_ownership(s.obj)),
      orientation(s.orientation) {}
  Structure_Instance& Structure_Instance::
  operator=(Structure_Instance const& i) noexcept
  {
    structure_type = i.structure_type;
    obj = share_object_keep_ownership(i.obj);
    orientation = i.orientation;
    return *this;
  }
}
