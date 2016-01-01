/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <utility> // for std::move
namespace redc
{
  template <class T> struct Maybe_Owned;

  /*!
   * Named to be consistent with make_shared and make_unique.
   */
  template <class T, class... Args>
  Maybe_Owned<T> make_maybe_owned(Args&&... args) noexcept
  {
    return Maybe_Owned<T>(new T(std::forward<Args>(args)...), true);
  }
  /*!
   * Named to contrast and stress the owned nature of the pointer passed in.
   */
  template <class T>
  Maybe_Owned<T> make_owned_maybe(T* ptr) noexcept
  {
    return Maybe_Owned<T>(ptr, true);
  }

  template <class T>
  Maybe_Owned<T> ref_mo(Maybe_Owned<T> const& mo) noexcept
  {
    return Maybe_Owned<T>{mo.get(), false};
  }

  // TODO Support a custom deleter.
  template <class T>
  struct Maybe_Owned
  {
    // ** PREFER THE ABOVE FUNCTION MAKE_MAYBE_OWNED **
    template <class... Args>
    explicit Maybe_Owned(Args&&... args) noexcept;

    explicit Maybe_Owned(T&& t) noexcept;

    /*!
     * \brief Construct the maybe owned with a borrowed/unowned ptr.
     *
     * \note If you are using this constructor to initialize a maybe owned
     * of type base with a new pointer to a derived instance, make sure to
     * set the second parameter to true. Better yet use make_maybe_owned
     * declared above! make_owned_maybe can also be used if the pointer
     * cannot be constructed by the client.
     */
    /* implicit */ Maybe_Owned(T* t, bool owned) noexcept;

    Maybe_Owned() noexcept;

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

    void set_pointer(T* t) noexcept;

    template <class R>
    void set_pointer(Maybe_Owned<R> const&) noexcept;

    template <class R, class... Args>
    void emplace_owned(Args&&... args) noexcept;

    T* get() const noexcept;
    T&& unwrap() noexcept;
    T* operator->() const noexcept;
    T& operator*() const noexcept;

    operator bool() const noexcept;

    bool is_owned() const noexcept;
    bool is_pointer() const noexcept;

    // TODO: Remove this function, it is inconsistent from the unique_ptr's
    // reset somehow, I think.
    void reset() noexcept;
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
  Maybe_Owned<T>::Maybe_Owned() noexcept : owned_(false), ptr_(nullptr) {}

  template <class T>
  template <class R>
  Maybe_Owned<T>::Maybe_Owned(std::unique_ptr<R> ptr) noexcept
    : owned_(true), ptr_(ptr.release()) {}

  template <class T>
  Maybe_Owned<T>::~Maybe_Owned() noexcept
  {
    reset();
  }

  template <class T>
  template <class R>
  Maybe_Owned<T>::Maybe_Owned(Maybe_Owned<R>&& mo1) noexcept
  {
    // Just use our operator=, since it's somewhat complex of an impl.
    *this = std::move(mo1);
  }

  template <class T>
  template <class R>
  Maybe_Owned<T>& Maybe_Owned<T>::operator=(std::unique_ptr<R> ptr) noexcept
  {
    reset();

    owned_ = true;
    ptr_ = ptr.release();

    return *this;
  }

  template <class T>
  template <class R>
  Maybe_Owned<T>& Maybe_Owned<T>::operator=(Maybe_Owned<R>&& mo1) noexcept
  {
    // If we have the same pointer, move ownership to ourselves if necessary.
    if(mo1.ptr_ == ptr_)
    {
      // If they owned it, now we do. That's about it, since neither pointer
      // needs to change.
      if(mo1.owned_)
      {
        owned_ = true;
        mo1.owned_ = false;
      }
    }
    else
    {
      // Release ourselves, possibly unallocating our own pointer (only if we
      // own it, of course).
      reset();

      // We own now if they did, but not if they didn't
      owned_ = mo1.owned_;
      ptr_ = mo1.ptr_;

      // We don't need to null their pointer, just kill their ownership, which
      // is enough. It also has the lovely side effect of leaving mo1 in a
      // non-owning but functionally-equivalent state!
      mo1.owned_ = false;
    }

    return *this;
  }

  template <class T>
  void Maybe_Owned<T>::set_owned(T&& t) noexcept
  {
    *this = Maybe_Owned<T>(new T(std::move(t)), true);
  }
  template <class T>
  void Maybe_Owned<T>::set_owned(T const& t) noexcept
  {
    *this = Maybe_Owned<T>(new T(t), true);
  }
  template <class T>
  void Maybe_Owned<T>::set_owned(T* t) noexcept
  {
    *this = Maybe_Owned<T>(t, true);
  }
  template <class T>
  void Maybe_Owned<T>::set_pointer(T* t) noexcept
  {
    *this = Maybe_Owned<T>(t, false);
  }
  template <class T>
  template <class R>
  void Maybe_Owned<T>::set_pointer(Maybe_Owned<R> const& mo) noexcept
  {
    *this = Maybe_Owned<T>(mo.get(), false);
  }

  template <class T>
  template <class R, class... Args>
  void Maybe_Owned<T>::emplace_owned(Args&&... args) noexcept
  {
    *this = make_maybe_owned<T>(std::forward<Args>(args)...);
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
    reset();
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
  template <class T>
  void Maybe_Owned<T>::reset() noexcept
  {
    if(owned_)
    {
      delete ptr_;
    }
    ptr_ = nullptr;
    owned_ = false;
  }
}
