/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "vec.h"
#include <cmath>
namespace game
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
      // Check if the angle of the point in the circle is in the arc.
      dif = normalize(dif);
      Vec<T> rel{static_cast<T>(1), static_cast<T>(0)};

      auto cos0 = dot(dif, rel);
      auto angle = std::acos(cos0);
      return circle.start_radians <= angle && angle <= circle.end_radians;
    }
    return false;
  }
}
