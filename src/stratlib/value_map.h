/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
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
  }
  template <class T>
  Value_Map<T>::~Value_Map() noexcept
  {
    delete[] values;
  }
} }
