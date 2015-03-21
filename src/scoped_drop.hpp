/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "irrlicht.h"
namespace survive
{
  template <class T>
  struct Scoped_Drop
  {
    inline Scoped_Drop(T* t) noexcept : t_(t) {}
    ~Scoped_Drop() noexcept;
  private:
    T* t_;
  };

  template <class T>
  Scoped_Drop<T>::~Scoped_Drop() noexcept
  {
    t_->drop();
  }

  template <class T>
  auto make_scoped_drop(T* t) noexcept
  {
    return Scoped_Drop<T>{t};
  }
}
