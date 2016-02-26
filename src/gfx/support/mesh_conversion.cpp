/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "mesh_conversion.h"
#include <tuple>
#include <set>
namespace redc { namespace gfx
{
  // Pronounded Ordered-vertex-reference.
  struct Ord_Vert_Ref
  {
    unsigned int where;
    Vert_Ref vert;
  };
  bool operator<(Ord_Vert_Ref const& lhs, Ord_Vert_Ref const& rhs) noexcept
  {
    return lhs.vert < rhs.vert;
  }
  bool operator==(Ord_Vert_Ref const& lhs, Ord_Vert_Ref const& rhs) noexcept
  {
    return lhs.vert == rhs.vert;
  }

  Indexed_Mesh_Data
  to_indexed_mesh_data(Indexed_Split_Mesh_Data const& data) noexcept
  {
    Indexed_Mesh_Data ret;

    using std::begin; using std::end;

    std::set<Ord_Vert_Ref> vert_set;
    unsigned int where = 0;

    // Insert each unique permutation of vertices (unique vert_ref instance)
    // into the set.
    for(Vert_Ref orig_v : data.indices)
    {
      Ord_Vert_Ref ref;
      ref.where = where;
      ref.vert = orig_v;

      auto insert_res = vert_set.insert(ref);
      if(std::get<1>(insert_res)) ++where;
    }

    auto set_with = [](auto& input_vert, auto Vertex::* member,
                       auto const& index_opt, auto const& vec)
    {
      if(index_opt)
      {
        auto val = index_opt.get();
        if(val < vec.size())
        {
          input_vert.*member = vec[val];
        }
      }
    };

    // Insert each necessary vertex. One for every element of the vert_set.
    ret.vertices.resize(vert_set.size());
    for(auto const& ref : vert_set)
    {
      Vertex vert;

      set_with(vert, &Vertex::position, ref.vert.position, data.positions);
      set_with(vert, &Vertex::normal, ref.vert.normal, data.normals);
      set_with(vert, &Vertex::uv, ref.vert.tex_coord, data.tex_coords);

      // Use the order from when each vert_ref was added to the set. This was
      // preserved in the Ord_Vert_Ref::where member. We need to know where
      // each vertex is no matter what (to convert indices / the element array)
      // so we might as well do it from the start. Otherwise we would have to
      // record the order here. That is, we need a way to find the index of a
      // given vert ref.
      ret.vertices[ref.where] = vert;
    }

    // Create an index list, by finding each permutation in the set.
    for(Vert_Ref orig_ref : data.indices)
    {
      Ord_Vert_Ref ref;
      ref.vert = orig_ref;
      auto ord_find = vert_set.find(ref);

      // This shouldn't even happen once.
      if(ord_find == vert_set.end()) continue;

      ret.elements.push_back(ord_find->where);
    }

    return ret;
  }
} }
