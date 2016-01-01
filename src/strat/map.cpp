/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "map.h"
#include "../collisionlib/sweep_and_prune.h"
#include "../common/log.h"
#include <glm/gtc/matrix_transform.hpp>

namespace redc { namespace strat
{
  Structure_Instance::Structure_Instance(Structure const& s, pos_t p) noexcept
    : pos_(p), s_type_(&s){}

  void Structure_Instance::set_structure_type(Structure const& s) noexcept
  {
    s_type_ = &s;
  }
  Structure const& Structure_Instance::structure() const noexcept
  {
    return *s_type_;
  }

  pos_t Structure_Instance::position() const noexcept
  {
    return pos_;
  }
  void Structure_Instance::position(pos_t const& pos) noexcept
  {
    pos_ = pos;
    model_cache_ = boost::none;
  }

  float Structure_Instance::y_rot() const noexcept
  {
    return y_rot_;
  }
  void Structure_Instance::y_rot(float y) noexcept
  {
    y_rot_ = y;
    model_cache_ = boost::none;
  }

  glm::mat4 Structure_Instance::model_matrix() const noexcept
  {
    if(!model_cache_)
    {
      auto model = glm::mat4(1.0f);

      auto ray = ray_to_structure_bottom_center(structure());
      model = glm::translate(model, -ray);

      model = glm::translate(model, glm::vec3(pos_.x, 0.0f, pos_.y));
      model = glm::rotate(model, y_rot_, glm::vec3(0.0f, 1.0f, 0.0f));

      model_cache_ = model;
    }
    return *model_cache_;
  }

  void render_structure(gfx::IDriver& d, Structure const& st,
                        glm::mat4 model) noexcept
  {
    d.bind_texture(*st.texture(), 0);
    d.active_shader()->set_model(model);

    gfx::render_chunk(st.mesh_chunk());
  }

  bool try_structure_place(Map& map, Structure const& st, pos_t pos,
                           gfx::Immediate_Renderer* debug) noexcept
  {
    // Check collision with every other structure.
    collis::SAP sap;
    std::vector<collis::SAP_AABB_Ref> ids;
    for(auto const& other_st : map.structures)
    {
      auto aabb = other_st.structure().aabb();
      aabb.min.x += other_st.position().x;
      aabb.min.z += other_st.position().y;
      aabb.min.y = 0.0f;
      ids.push_back(sap.insert(aabb));

      // Make this debugging bit less intrusive.
      if(debug) debug->draw_aabb(aabb);
    }

    auto aabb = st.aabb();
    aabb.min.x += pos.x;
    aabb.min.z += pos.y;
    aabb.min.y = 0.0f;
    collis::SAP_AABB_Ref this_id = sap.insert(aabb);
    if(debug) debug->draw_aabb(aabb);

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
} }
