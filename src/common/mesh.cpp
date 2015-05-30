/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "mesh.h"
#include <set>
#include <functional>
namespace game
{
  Mesh_Chunk Mesh_Data::append(Mesh_Data const& md) noexcept
  {
    // Should we check to make sure hints and primitive type are the same? Ehh.

    vertices.insert(vertices.end(), md.vertices.begin(), md.vertices.end());

    auto initial_size = elements.size();
    for(unsigned int index : md.elements)
    {
      elements.push_back(index + initial_size);
    }

    Mesh_Chunk chunk;
    chunk.offset = initial_size;
    chunk.count = md.elements.size();
    return chunk;
  }
  void Mesh::allocate(unsigned int max_verts, unsigned int max_elemnt_indices,
                Usage_Hint ush, Upload_Hint uph, Primitive_Type p) noexcept
  {
    allocate_(max_verts, max_elemnt_indices, ush, uph, p);
    allocated_ = true;
  }
  void Mesh::allocate_from(Mesh_Data const& md) noexcept
  {
    allocate_from_(md);
    allocated_ = true;
  }
  void Mesh::allocate_from(Mesh_Data&& md) noexcept
  {
    allocate_from_(std::move(md));
    allocated_ = true;
  }
  void Mesh::allocate_from_(Mesh_Data&& mesh) noexcept
  {
    // By default initiate a copy of this data.
    allocate_from(static_cast<Mesh_Data const&>(mesh));
  }
  void Mesh::allocate_from_(Mesh_Data const& md) noexcept
  {
    allocate(md.vertices.size(), md.elements.size(), md.usage_hint,
             md.upload_hint, md.primitive);
    set_vertices(0, md.vertices.size(), &md.vertices[0]);
    set_element_indices(0, md.elements.size(), &md.elements[0]);
    set_num_element_indices(md.elements.size());
  }
  AABB generate_aabb(Mesh_Data const& mesh) noexcept
  {
    glm::vec3 min;
    glm::vec3 max;

    for(auto const& vertex : mesh.vertices)
    {
      min.x = std::min(min.x, vertex.position.x);
      min.y = std::min(min.y, vertex.position.y);
      min.z = std::min(min.z, vertex.position.z);

      max.x = std::max(max.x, vertex.position.x);
      max.y = std::max(max.y, vertex.position.y);
      max.z = std::max(max.z, vertex.position.z);
    }

    auto aabb = AABB{};
    aabb.width = std::abs(max.x - min.x);
    aabb.height = std::abs(max.y - min.y);
    aabb.depth = std::abs(max.z - min.z);

    aabb.min = min;

    return aabb;
  }

  bool operator==(Vert_Ref const& lhs, Vert_Ref const& rhs) noexcept
  {
    return lhs.position == rhs.position &&
           lhs.normal == rhs.normal &&
           lhs.tex_coord == rhs.tex_coord;
  }
  bool operator<(Vert_Ref const& lhs, Vert_Ref const& rhs) noexcept
  {
    if(lhs.position < rhs.position) return true;
    else if(lhs.position == rhs.position)
    {
      if(lhs.normal < rhs.normal) return true;
      else if(lhs.normal == rhs.normal)
      {
        if(lhs.tex_coord < rhs.tex_coord) return true;
        else return false;
      }
    }
    return false;
  }


  // Pronounded Ordered-vertex-reference.
  using Ord_Vert_Ref = std::pair<unsigned int, Vert_Ref>;
  bool operator<(Ord_Vert_Ref const& lhs, Ord_Vert_Ref const& rhs) noexcept
  {
    return std::get<1>(lhs) < std::get<1>(rhs);
  }
  bool operator==(Ord_Vert_Ref const& lhs, Ord_Vert_Ref const& rhs) noexcept
  {
    return std::get<1>(lhs) == std::get<1>(rhs);
  }

  template <class T, class F>
  void if_has_value(boost::optional<T> const& t, F func) noexcept
  {
    if(t)
    {
      func(t.value());
    }
  }

  Mesh_Data make_optimized_mesh_data(std::vector<Vert_Ref> orig_indices,
                                     std::vector<glm::vec3> positions,
                                     std::vector<glm::vec3> normals,
                                     std::vector<glm::vec2> tex_coords)noexcept
  {
    using std::begin; using std::end;

    // The set represents all unique vertices.
    std::set<Ord_Vert_Ref> indices;
    unsigned int where = 0;
    for(Vert_Ref orig_v : orig_indices)
    {
      Ord_Vert_Ref ref;
      std::get<0>(ref) = where;
      std::get<1>(ref) = orig_v;

      auto res = indices.insert(ref);
      if(std::get<1>(res)) ++where;
    }

    Mesh_Data ret;

    // Insert each necessary, unique permutations of vertex arrays.
    ret.vertices.resize(indices.size());
    for(auto const& ref : indices)
    {
      Vertex vert;

      if_has_value(std::get<1>(ref).position, [&](auto pos)
      {
        pos -= 1;
        if(pos < positions.size())
        {
          vert.position = positions[pos];
        }
      });
      if_has_value(std::get<1>(ref).normal, [&](auto norm)
      {
        norm -= 1;
        if(norm < normals.size())
        {
          vert.normal = normals[norm];
        }
      });
      if_has_value(std::get<1>(ref).tex_coord, [&](auto uv)
      {
        uv -= 1;
        if(uv < tex_coords.size())
        {
          vert.uv = tex_coords[uv];
        }
      });

      // Set the vertices in order of their index (first element of the tuple).
      // This may benefit from a cache optimization, but probably not. Since
      // probably fragmentation of the set itself is unavoidable.
      ret.vertices[std::get<0>(ref)] = vert;
    }

    // Create an index list, by finding each permutation in the set.
    for(Vert_Ref orig_ref : orig_indices)
    {
      Ord_Vert_Ref ref;
      std::get<1>(ref) = orig_ref;
      auto ord_find = indices.find(ref);

      // This shouldn't even happen once.
      if(ord_find == indices.end()) continue;

      ret.elements.push_back(std::get<0>(*ord_find));
    }

    ret.primitive = Primitive_Type::Triangle;

    return ret;
  }
}
