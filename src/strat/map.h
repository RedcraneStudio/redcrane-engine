/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../common/vec.h"

#include "wall.h"
#include "structure.h"

#include "../gfx/immediate_renderer.h"
namespace game { namespace strat
{
  using pos_t = Vec<float>;

  struct Structure_Instance
  {
    Structure_Instance(Structure const&, pos_t pos) noexcept;
    ~Structure_Instance() noexcept = default;

    void set_structure_type(Structure const& s) noexcept;
    Structure const& structure() const noexcept;

    pos_t position() const noexcept;
    void position(pos_t const&) noexcept;

    // Angle in radians
    float y_rot() const noexcept;
    void y_rot(float) noexcept;

    glm::mat4 model_matrix() const noexcept;
  private:
    mutable boost::optional<glm::mat4> model_cache_;

    pos_t pos_;
    float y_rot_;

    Structure const* s_type_;
  };

  void render_structure(gfx::IDriver& d, Structure const& st,
                        glm::mat4 mat) noexcept;
  inline void render_structure_instance(gfx::IDriver& d,
                                        Structure_Instance const& st) noexcept
  {
    render_structure(d, st.structure(), st.model_matrix());
  }

  struct Map
  {
    Map(Vec<float> map_extents) noexcept : extents(map_extents) {}

    boost::optional<Structure_Instance> pending_structure;
    std::vector<Structure_Instance> structures;

    Vec<float> extents;

    boost::optional<Pending_Wall> pending_wall;
    std::vector<Wall> walls;
  };

  bool try_structure_place(Map& map, Structure const& st, pos_t pos,
                           gfx::Immediate_Renderer* = nullptr) noexcept;
} }
