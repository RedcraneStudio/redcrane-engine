/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "vec.h"
#include <cmath>
#include "pi.h"
namespace redc
{
  template <class T>
  struct Circle
  {
    Vec<T> center;
    T radius;
  };

  template <class T, class Rad_T = float>
  struct Arc
  {
    Vec<T> center;
    T radius;

    Rad_T start_radians;
    Rad_T end_radians;
  };

  template <class T>
  bool is_in(Circle<T> const& circle, Vec<T> pt) noexcept
  {
    return length(pt - circle.center) <= circle.radius;
  }
  template <class T>
  bool is_in(Arc<T> const& circle, Vec<T> pt) noexcept
  {
    // Gah, floating pointe equality. Fuck it, it's the only case that doesn't
    // work.
    if(pt == circle.center) return true;

    auto dif = pt - circle.center;
    if(length(dif) <= circle.radius && length(dif) != 0)
    {
      auto dir = normalize(dif);
      auto angle = std::atan2(dir.y, dir.x);
      if(angle < 0) angle = (2 * REDC_PI) + angle;

      if(circle.end_radians < circle.start_radians)
      {
        return circle.start_radians <= angle || angle <= circle.end_radians;
      }
      else
      {
        return circle.start_radians <= angle && angle <= circle.end_radians;
      }
    }
    return false;
  }
}
