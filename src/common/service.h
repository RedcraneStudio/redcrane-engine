/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
namespace redc
{
  template <class T>
  struct Service
  {
    static T* get() noexcept;
    static void set(std::unique_ptr<T>) noexcept;

    static std::unique_ptr<T> ptr;
  };

  template <class T>
  T* Service<T>::get() noexcept
  {
    return ptr.get();
  };

  template <class T>
  void Service<T>::set(std::unique_ptr<T> np) noexcept
  {
    ptr = std::move(np);
  }
}
