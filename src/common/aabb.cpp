/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "aabb.h"
namespace game
{
  AABB aabb_from_min_max(glm::vec3 min, glm::vec3 max) noexcept
  {
    auto aabb = AABB{};
    aabb.width = std::abs(max.x - min.x);
    aabb.height = std::abs(max.y - min.y);
    aabb.depth = std::abs(max.z - min.z);

    aabb.min = min;
    return aabb;
  }

  glm::vec3 min_pt(glm::vec3 v1, glm::vec3 v2) noexcept
  {
    glm::vec3 pt;

    pt.x = std::min(v1.x, v2.x);
    pt.y = std::min(v1.y, v2.y);
    pt.z = std::min(v1.z, v2.z);

    return pt;
  }
  glm::vec3 max_pt(glm::vec3 v1, glm::vec3 v2) noexcept
  {
    glm::vec3 pt;

    pt.x = std::max(v1.x, v2.x);
    pt.y = std::max(v1.y, v2.y);
    pt.z = std::max(v1.z, v2.z);

    return pt;
  }

  glm::vec3 ray_to_aabb_bottom_center(AABB const& a) noexcept
  {
    // Find bottom center point.
    auto bcp = a.min;

    bcp.x += a.width / 2;
    bcp.z += a.depth / 2;

    // From (0, 0, 0) (model origin) to bcp.
    // return bcp - glm::vec3(0.0f, 0.0f, 0.0f) or just:
    return bcp;
  }

  Point_Iter::Point_Iter(AABB const* aabb) noexcept : aabb_(aabb)
  {
    if(aabb == nullptr)
    {
      where_ = pts_.cend();
      return;
    }

    where_ = pts_.cbegin();

    pts_[0] = glm::vec3(aabb->min.x,
                        aabb->min.y,
                        aabb->min.z);

    pts_[1] = glm::vec3(aabb->min.x + aabb->width,
                        aabb->min.y,
                        aabb->min.z);

    pts_[2] = glm::vec3(aabb->min.x,
                        aabb->min.y + aabb->height,
                        aabb->min.z);

    pts_[3] = glm::vec3(aabb->min.x,
                        aabb->min.y,
                        aabb->min.z + aabb->depth);

    pts_[4] = glm::vec3(aabb->min.x + aabb->width,
                        aabb->min.y + aabb->height,
                        aabb->min.z);

    pts_[5] = glm::vec3(aabb->min.x + aabb->width,
                        aabb->min.y,
                        aabb->min.z + aabb->depth);

    pts_[6] = glm::vec3(aabb->min.x,
                        aabb->min.y + aabb->height,
                        aabb->min.z + aabb->depth);

    pts_[7] = glm::vec3(aabb->min.x + aabb->width,
                        aabb->min.y + aabb->height,
                        aabb->min.z + aabb->depth);
  }
  glm::vec3 const& Point_Iter::operator*() const noexcept
  {
    // crash if we are the end iterator.
    return *where_;
  }
  Point_Iter& Point_Iter::operator++() noexcept
  {
    ++where_;
    return *this;
  }
  bool Point_Iter::operator!=(Point_Iter const& pi) const noexcept
  {
    return !(*this == pi);
  }
  bool Point_Iter::operator==(Point_Iter const& pi) const noexcept
  {
    return this->where_ == pi.where_;
  }
}
