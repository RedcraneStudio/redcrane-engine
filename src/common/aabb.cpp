/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "aabb.h"
namespace game
{
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
    return this->where_ = pi.where_;
  }
}
