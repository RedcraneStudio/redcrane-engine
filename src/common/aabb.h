/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <array>
#include <glm/glm.hpp>
namespace game
{
  struct AABB
  {
    float depth;
    float height;
    float width;

    glm::vec3 min;
  };

  struct Point_Iter
  {
    Point_Iter(AABB const* aabb = nullptr) noexcept;

    glm::vec3 const& operator*() const noexcept;
    Point_Iter& operator++() noexcept;

    bool operator==(Point_Iter const& pi) const noexcept;
    bool operator!=(Point_Iter const& pi) const noexcept;
  private:
    AABB const* aabb_;
    std::array<glm::vec3, 8> pts_;
    std::array<glm::vec3, 8>::const_iterator where_;
  };

  inline Point_Iter begin_point_iter(AABB const& aabb) noexcept
  { return Point_Iter(&aabb); }

  inline Point_Iter end_point_iter() noexcept
  { return Point_Iter{}; }
}
