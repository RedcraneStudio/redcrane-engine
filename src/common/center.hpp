/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
namespace game
{
  /*!
   * \brief Gets the point which should be the location of an object of
   * `object_length` with the bounds which begin at `begin` and go on for
   * `length`.
   *
   * \param begin The location where the line, etc begins.
   * \param length How far this line, etc, goes for, after `begin`.
   * \param object_length The length of the object which needs to be centered.
   *
   * \return The 1d point which should be the position of an object of length
   * `object_length` if it needs to be centered in a line starting at point
   * `begin` going on for `length` amount.
   *
   * \note Example: If begin==100, length==100, and object_length==50. This
   * function returns 125. This would be the x or y point for on object of
   * length 50 which needs to be centered within the bounds 100-200.
   *
   * \note The algorithm is exactly:
   * \code begin + (length / 2) - (object_length / 2) \endcode
   * This is what is returned.
   */
  template<typename P1, typename P2, typename P3>
  inline constexpr decltype(auto)
  center(P1 begin, P2 length, P3 object_length)
  {
    return begin + (length / 2) - (object_length / 2);
  }
}
