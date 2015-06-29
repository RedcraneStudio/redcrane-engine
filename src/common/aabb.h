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

  AABB aabb_from_min_max(glm::vec3 min, glm::vec3 max) noexcept;
  glm::vec3 min_pt(glm::vec3 v1, glm::vec3 v2) noexcept;
  glm::vec3 max_pt(glm::vec3 v1, glm::vec3 v2) noexcept;

  glm::vec3 ray_to_aabb_bottom_center(AABB const& a) noexcept;

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
