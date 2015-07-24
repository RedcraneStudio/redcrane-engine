/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <cstring>
#include "../common/vec.h"
namespace game { namespace strat
{
  template <class T>
  struct Value_Map
  {
    void allocate(Vec<int> const& extents) noexcept;
    ~Value_Map() noexcept;

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

    std::memset(values, 0x00, area(e));
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
} }
