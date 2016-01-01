/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <cstring>
#include "vec.h"
#include "volume.h"
namespace redc
{
  template <class T>
  struct Value_Map
  {
    void allocate(Vec<int> const& extents) noexcept;
    ~Value_Map() noexcept;

    // We could be using std::size_t here instead of int and it would probably
    // be more correct, but that is a breaking change and dealing with unsigned
    // numbers is a pain. It's probably better to have good faith negative
    // numbers won't be used for the sake of simplicity.
    inline T& at(Vec<int> pos) noexcept
    {
      return values[pos.y * extents.x + pos.x];
    }
    inline T const& at(Vec<int> pos) const noexcept
    {
      return values[pos.y * extents.x + pos.x];
    }

    T* values = nullptr;
    Vec<int> extents = Vec<int>{0,0};
    bool allocated = false;
  };

  template <class T>
  void Value_Map<T>::allocate(Vec<int> const& e) noexcept
  {
    values = new T[area(e)];
    extents = e;
    allocated = true;

    std::memset(values, 0x00, area(e) * sizeof(T));
  }
  template <class T>
  Value_Map<T>::~Value_Map() noexcept
  {
    delete[] values;
  }

  template <class T>
  static bool is_in_bounds_of(Value_Map<T> const& vm, Vec<int> pos) noexcept
  {
    return 0 <= pos.x && pos.x < vm.extents.x &&
           0 <= pos.y && pos.y < vm.extents.y;
  };

  /*!
   * \brief Blur / Sample / Average a small section of the given value map.
   *
   * \param map The source value map.
   * \param vol The volume in value map coordinates.
   *
   * \return The average of all values in the volume given.
   */
  template <class T>
  T sample(Value_Map<T> const& map, Volume<int> vol) noexcept
  {
    T sum = 0;

    for(auto i = vol.pos.y; i < vol.pos.y + vol.height; ++i)
    {
      for(auto j = vol.pos.x; j < vol.pos.x + vol.width; ++j)
      {
        sum += map.at({j,i});
      }
    }

    return sum / (vol.width * vol.height);
  }
}
