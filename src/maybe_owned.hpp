/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <utility> // for std::move
namespace survive
{
  template <class T>
  struct Maybe_Owned
  {
    template <class... Args>
    Maybe_Owned(Args&&... args) noexcept;

    Maybe_Owned(T&& t) noexcept;
    Maybe_Owned(T* t = nullptr) noexcept;

    template <class R>
    Maybe_Owned(Maybe_Owned<R>&&) noexcept;
    Maybe_Owned(Maybe_Owned const&) noexcept = delete;

    template <class R>
    Maybe_Owned& operator=(Maybe_Owned<R>&&) noexcept;
    Maybe_Owned& operator=(Maybe_Owned const&&) noexcept = delete;

    ~Maybe_Owned() noexcept;

    void set_owned(T&& t) noexcept;
    void set_pointer(T* t) noexcept;

    T const* get() const noexcept;
    T* get() noexcept;

    T&& unwrap() noexcept;

    T const* operator->() const noexcept;
    T* operator->() noexcept;

    T const& operator*() const noexcept;
    T& operator*() noexcept;

    operator bool() const noexcept;

    bool is_owned() const noexcept;
    bool is_pointer() const noexcept;
  private:
    bool owned_ = false;
    T* ptr_ = nullptr;
  };

  template <class T>
  template <class... Args>
  Maybe_Owned<T>::Maybe_Owned(Args&&... args) noexcept
    : owned_(true), ptr_(new T{std::forward<Args>(args)...}) {}

  template <class T>
  Maybe_Owned<T>::Maybe_Owned(T&& t) noexcept
    : owned_(true), ptr_(new T(std::move(t))) {}

  template <class T>
  Maybe_Owned<T>::Maybe_Owned(T* t) noexcept : owned_(false), ptr_(t) {}

  template <class T>
  Maybe_Owned<T>::~Maybe_Owned() noexcept
  {
    if(owned_) delete ptr_;
  }

  template <class T>
  template <class R>
  Maybe_Owned<T>::Maybe_Owned(Maybe_Owned<R>&& mo1) noexcept
    : owned_(mo1.owned_), ptr_(mo1.ptr_)
  {
    mo1.owned_ = false;
    // We don't need to null the pointer since setting the owned value to false
    // should suffice in preventing this other maybe-owned from deleting its
    // pointer. Nonetheless:
    mo1.ptr_ = nullptr;
  }

  template <class T>
  template <class R>
  Maybe_Owned<T>& Maybe_Owned<T>::operator=(Maybe_Owned<R>&& mo1) noexcept
  {
    owned_ = mo1.owned_;
    ptr_ = mo1.ptr_;

    mo1.owned_ = false;
    mo1.ptr_ = nullptr;
  }

  template <class T>
  void Maybe_Owned<T>::set_owned(T&& t) noexcept
  {
    owned_ = true;
    ptr_ = new T(std::move(t));
  }
  template <class T>
  void Maybe_Owned<T>::set_pointer(T* t) noexcept
  {
    owned_ = false;
    ptr_ = t;
  }

  template <class T>
  T const* Maybe_Owned<T>::get() const noexcept
  {
    return ptr_;
  }
  template <class T>
  T* Maybe_Owned<T>::get() noexcept
  {
    return ptr_;
  }

  template <class T>
  T&& Maybe_Owned<T>::unwrap() noexcept
  {
    owned_ = false;
    T&& old_t = std::move(*ptr_);
    ptr_ = nullptr;
    return std::move(old_t);
  }

  template <class T>
  T const* Maybe_Owned<T>::operator->() const noexcept
  {
    return ptr_;
  }
  template <class T>
  T* Maybe_Owned<T>::operator->() noexcept
  {
    return ptr_;
  }

  template <class T>
  T const& Maybe_Owned<T>::operator*() const noexcept
  {
    return *ptr_;
  }
  template <class T>
  T& Maybe_Owned<T>::operator*() noexcept
  {
    return *ptr_;
  }

  template <class T>
  bool Maybe_Owned<T>::is_owned() const noexcept
  {
    return owned_;
  }
  template <class T>
  bool Maybe_Owned<T>::is_pointer() const noexcept
  {
    return !owned_;
  }

  template <class T>
  Maybe_Owned<T>::operator bool() const noexcept
  {
    return get();
  }
}
