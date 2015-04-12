/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "map.h"
#include "../gfx/idriver.h"
namespace strat
{
  struct Json_Structure : public IStructure
  {
    Json_Structure(std::string) noexcept;

    AABB aabb() const noexcept override;

    gfx::Object const& obj() const noexcept override;
    void prepare(gfx::IDriver&) noexcept override;

    void set_model(glm::mat4 const&) noexcept override;
  private:
    glm::mat4 gen_model_() const noexcept;
    gfx::Object obj_;
    AABB aabb_;
  };
}
