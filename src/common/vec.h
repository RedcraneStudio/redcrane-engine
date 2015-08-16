/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once

#include <cmath>
namespace game
{
  template <typename T = int>
  struct Vec
  {
    Vec(T const& x = 0, T const& y = 0) noexcept : x(x), y(y) {}

    template <typename P2>
    Vec(Vec<P2> const& vec) noexcept : x(vec.x), y(vec.y) {}

    T x;
    T y;
  };

  template <class T1, class T2>
  bool operator<(Vec<T1> const& t1, Vec<T2> const& t2) noexcept
  {
    if(t1.x == t2.x) return t1.y < t2.y;
    return t1.x < t2.x;
  }

  template <class Ret, class Param>
  Vec<Ret> vec_cast(Vec<Param> const& v) noexcept
  {
    return {static_cast<Ret>(v.x), static_cast<Ret>(v.y)};
  }

  // comparison operators
  template <typename P1, typename P2>
  inline auto operator==(Vec<P1> const& p1, Vec<P2> const& p2) noexcept -> bool
  {
    return p1.x == p2.x && p1.y == p2.y;
  }
  template <typename P1, typename P2>
  inline auto operator!=(Vec<P1> const& p1, Vec<P2> const& p2) noexcept -> bool
  {
    return !(p1 == p2);
  }

  template <typename ret_t = double, typename P>
  inline ret_t length(Vec<P> const& vec) noexcept
  {
    ret_t x = static_cast<ret_t>(vec.x);
    ret_t y = static_cast<ret_t>(vec.y);

    x *= x;
    y *= y;

    return std::sqrt(x + y);
  }

  template <typename Ret_Point_T = double, typename P>
  inline auto normalize(Vec<P> const& vec) noexcept -> decltype(auto)
  {
    Vec<Ret_Point_T> result;

    auto vec_len = length<Ret_Point_T>(vec);
    if(vec_len == 0.0) return result;

    result.x = vec.x / vec_len;
    result.y = vec.y / vec_len;

    return result;
  }

  // fun stuff
  template <typename P, typename Angle_T>
  inline auto rotate(Vec<P> const& vec, Angle_T angle) noexcept -> Vec<P>
  {
    auto ret = Vec<P>{};

    ret.x = (vec.x * std::cos(angle)) - (vec.y * std::sin(angle));
    ret.y = (vec.x * std::sin(angle)) + (vec.y * std::cos(angle));

    return ret;
  }

  // addition
  template <typename P1, typename P2> inline auto
  operator+(Vec<P1> const& lhs, Vec<P2> const& rhs) noexcept -> decltype(auto)
  {
    return Vec<decltype(P1() + P2())>{lhs.x + rhs.x, lhs.y + rhs.y};
  }
  template <typename P1, typename P2> inline auto
  operator+=(Vec<P1>& lhs, Vec<P2> const& rhs) noexcept -> Vec<P1>&
  {
    lhs.x += rhs.x;
    lhs.y += rhs.y;

    return lhs;
  }

  // subtraction
  template <typename P1, typename P2> inline auto
  operator-(Vec<P1> const& lhs, Vec<P2> const& rhs) noexcept -> decltype(auto)
  {
    return Vec<decltype(P1() - P2())>{lhs.x - rhs.x, lhs.y - rhs.y};
  }
  template <typename P1, typename P2> inline auto
  operator-=(Vec<P1>& lhs, Vec<P2> const& rhs) noexcept -> Vec<P1>&
  {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;

    return lhs;
  }

  // multiplication with a scalar
  template <typename P1, typename Scalar> inline auto
  operator*(Vec<P1> const& lhs, Scalar rhs) noexcept -> Vec<P1>
  {
    return Vec<P1>{lhs.x * rhs, lhs.y * rhs};
  }
  template <typename P1, typename Scalar> inline auto
  operator*=(Vec<P1>& lhs, Scalar rhs) noexcept -> Vec<P1>&
  {
    lhs.x *= rhs;
    lhs.y *= rhs;

    return lhs;
  }

  // division with a scalar
  template <typename P1, typename Scalar> inline auto
  operator/(Vec<P1> const& lhs, Scalar rhs) noexcept -> Vec<P1>
  {
    return Vec<P1>{lhs.x / rhs, lhs.y / rhs};
  }
  template <typename P1, typename Scalar> inline auto
  operator/=(Vec<P1>& lhs, Scalar rhs) noexcept -> Vec<P1>&
  {
    lhs.x /= rhs;
    lhs.y /= rhs;

    return lhs;
  }

  template <typename P1>
  inline auto area(Vec<P1> const& extents) noexcept -> decltype(auto)
  {
    return extents.x * extents.y;
  }

  template <typename T1, typename T2> inline auto
  dot(Vec<T1> const& v1, Vec<T2> const& v2) noexcept -> decltype(auto)
  {
    return v1.x * v2.x + v1.y * v2.y;
  }

  template <typename T> inline auto
  project_onto_pt_axes(Vec<T> initial, Vec<T> endpt) noexcept -> decltype(auto)
  {
    // Direction to end point from initial.
    auto to_endpt_dir = normalize(endpt - initial);

    // Find that angle.
    auto angle = std::atan2(to_endpt_dir.y, to_endpt_dir.x);

    Vec<float> ret{};

    if(-M_PI / 4 < angle && angle < M_PI / 4)
    {
      ret.x = 1.0f;
      ret.y = 0.0f;
    }
    else if(M_PI / 4 < angle && angle < M_PI * 3 / 4)
    {
      ret.x = 0.0f;
      ret.y = 1.0f;
    }
    else if(-M_PI * 3 / 4 < angle && angle < -M_PI / 4)
    {
      ret.x = 0.0f;
      ret.y = -1.0f;
    }
    else
    {
      ret.x = -1.0f;
      ret.y = 0.0f;
    }

    // We don't want to normalize this one
    auto to_endpt_full = endpt - initial;
    return ret * dot(ret, to_endpt_full) + initial;
  }
}
