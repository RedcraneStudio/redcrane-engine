/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "map.h"
#include "../gfx/idriver.h"
namespace game
{
  struct Json_Structure : public IStructure
  {
    Json_Structure(std::string) noexcept;

    AABB aabb() const noexcept override;

    gfx::Object make_obj() const noexcept override;
  private:
    gfx::Object obj_;
    AABB aabb_;
  };
}
