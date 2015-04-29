/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <utility> // for std::move
namespace game
{
  template <class T> struct Maybe_Owned;

  template <class T, class... Args>
  Maybe_Owned<T> make_maybe_owned(Args&&... args) noexcept
  {
    return Maybe_Owned<T>(new T(std::forward<Args>(args)...), true);
  }

  template <class T>
  struct Maybe_Owned
  {
    template <class... Args>
    explicit Maybe_Owned(Args&&... args) noexcept;

    explicit Maybe_Owned(T&& t) noexcept;

    /*!
     * \brief Construct the maybe owned with a borrowed/unowned ptr.
     *
     * \note If you are using this constructor to initialize a maybe owned
     * of type base with a new pointer to a derived instance, make sure to
     * set the second parameter to true. Better yet use make_maybe_owned
     * declared above!
     */
    /* implicit */ Maybe_Owned(T* t = nullptr, bool owned = false) noexcept;

    template <class R>
    Maybe_Owned(std::unique_ptr<R>) noexcept;

    template <class R>
    Maybe_Owned(Maybe_Owned<R>&&) noexcept;
    Maybe_Owned(Maybe_Owned const&) noexcept = delete;

    template <class R>
    Maybe_Owned& operator=(std::unique_ptr<R>) noexcept;

    template <class R>
    Maybe_Owned& operator=(Maybe_Owned<R>&&) noexcept;
    Maybe_Owned& operator=(Maybe_Owned const&&) noexcept = delete;

    ~Maybe_Owned() noexcept;

    void set_owned(T&& t) noexcept;
    void set_owned(T const& t) noexcept;
    void set_owned(T* t) noexcept;
    void set_pointer(T* t, bool owned = false) noexcept;

    template <class R, class... Args>
    void emplace_owned(Args&&... args) noexcept;

    T* get() const noexcept;
    T&& unwrap() noexcept;
    T* operator->() const noexcept;
    T& operator*() const noexcept;

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
  Maybe_Owned<T>::Maybe_Owned(T* t, bool o) noexcept : owned_(o), ptr_(t) {}

  template <class T>
  template <class R>
  Maybe_Owned<T>::Maybe_Owned(std::unique_ptr<R> ptr) noexcept
    : owned_(true), ptr_(ptr.release()) {}

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
  Maybe_Owned<T>& Maybe_Owned<T>::operator=(std::unique_ptr<R> ptr) noexcept
  {
    owned_ = true;
    ptr_ = ptr.release();

    return *this;
  }

  template <class T>
  template <class R>
  Maybe_Owned<T>& Maybe_Owned<T>::operator=(Maybe_Owned<R>&& mo1) noexcept
  {
    owned_ = mo1.owned_;
    ptr_ = mo1.ptr_;

    mo1.owned_ = false;
    mo1.ptr_ = nullptr;

    return *this;
  }

  template <class T>
  void Maybe_Owned<T>::set_owned(T&& t) noexcept
  {
    owned_ = true;
    ptr_ = new T(std::move(t));
  }
  template <class T>
  void Maybe_Owned<T>::set_owned(T const& t) noexcept
  {
    owned_ = true;
    ptr_ = new T(t);
  }
  template <class T>
  void Maybe_Owned<T>::set_owned(T* t) noexcept
  {
    owned_ = true;
    ptr_ = t;
  }
  template <class T>
  void Maybe_Owned<T>::set_pointer(T* t, bool o) noexcept
  {
    owned_ = o;
    ptr_ = t;
  }

  template <class T>
  template <class R, class... Args>
  void Maybe_Owned<T>::emplace_owned(Args&&... args) noexcept
  {
    owned_ = true;
    ptr_ = new R(std::forward<Args>(args)...);
  }

  template <class T>
  T* Maybe_Owned<T>::get() const noexcept
  {
    return ptr_;
  }

  template <class T>
  T&& Maybe_Owned<T>::unwrap() noexcept
  {
    T&& old_t = std::move(*ptr_);
    if(owned_)
    {
      delete ptr_;
    }
    ptr_ = nullptr;
    owned_ = false;
    return std::move(old_t);
  }

  template <class T>
  T* Maybe_Owned<T>::operator->() const noexcept
  {
    return ptr_;
  }
  template <class T>
  T& Maybe_Owned<T>::operator*() const noexcept
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
