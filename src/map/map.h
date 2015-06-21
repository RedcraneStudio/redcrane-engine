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
    Structure_Instance(Structure&, pos_t pos) noexcept;
    ~Structure_Instance() noexcept = default;

    void set_structure_type(Structure const& s) noexcept;
    Structure const& structure() noexcept;

    pos_t position;
  private:
    Structure const* s_type_;
  };

  struct Map
  {
    // Physical size on the screen, sort of. Hopefully can be used to make more
    // sense of input mouse coordinates.
    float pixels_per_unit;

    std::vector<Structure_Instance> structures;
  };
}
