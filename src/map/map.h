/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../common/vec.h"
#include "structure.h"
namespace game
{
  using pos_t = Vec<float>;

  struct Structure_Instance
  {
    Structure_Instance(Structure const&, pos_t pos) noexcept;
    ~Structure_Instance() noexcept = default;

    void set_structure_type(Structure const& s) noexcept;
    Structure const& structure() const noexcept;

    pos_t position;
  private:
    Structure const* s_type_;
  };

  struct Map
  {
    Map(Vec<float> map_extents) noexcept : extents(map_extents) {}

    std::vector<Structure_Instance> structures;

    Vec<float> extents;
  };

  bool try_structure_place(Map& map, Structure const& st, pos_t pos) noexcept;
}
