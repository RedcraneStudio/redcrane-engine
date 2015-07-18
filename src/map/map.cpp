/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "map.h"
#include "../collisionlib/sweep_and_prune.h"
#include "../common/log.h"

namespace game
{
  Structure_Instance::Structure_Instance(Structure const& s, pos_t p) noexcept
    : position(p), s_type_(&s){}

  void Structure_Instance::set_structure_type(Structure const& s) noexcept
  {
    s_type_ = &s;
  }
  Structure const& Structure_Instance::structure() const noexcept
  {
    return *s_type_;
  }

  bool try_structure_place(Map& map, Structure const& st, pos_t pos) noexcept
  {
    // Check collision with every other structure.
    SAP sap;
    std::vector<SAP_AABB_Ref> ids;
    for(auto const& other_st : map.structures)
    {
      auto aabb = other_st.structure().aabb();
      aabb.min.x += other_st.position.x;
      aabb.min.z += other_st.position.y;
      aabb.min.y = 0.0f;
      ids.push_back(sap.insert(aabb));
    }

    auto aabb = st.aabb();
    aabb.min.x += pos.x;
    aabb.min.z += pos.y;
    aabb.min.y = 0.0f;
    SAP_AABB_Ref this_id = sap.insert(aabb);

    auto pairs = sap.collisions();

    for(auto pair : pairs)
    {
      if(pair.first == this_id || pair.second == this_id)
      {
        // That's a collision
        return false;
      }
      // Otherwise let it go through.
    }

    // Make sure it is in the bounds of the map

    // Any other constraints?

    map.structures.emplace_back(st, pos);

    return true;
  }
}
