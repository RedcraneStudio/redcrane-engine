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
}
