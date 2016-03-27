/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#pragma once
namespace redc
{
  // We have a lot of resources that don't need to be kept alive by the engine,
  // but do need to be properly uninitialized before the engine is
  // uninitialized itself. We can achieve this by sharing the ownership of the
  // pointer between peer objects. One of them goes out of scope? Deallocate
  // the data immediately.

  template <class T>
  struct Peer_Ptr
  {
    ~Peer_Ptr();

    Peer_Ptr(Peer_Ptr&& ptr);
    Peer_Ptr(Peer_Ptr const&) = delete;

    Peer_Ptr& operator=(Peer_Ptr&& ptr);
    Peer_Ptr& operator=(Peer_Ptr const& ptr) = delete;

    T* operator->();
    T& operator*();

    T* get() const;

  private:
    Peer_Ptr() {}

    T** ptr_;

    template <class U, class... Args>
    friend std::pair<Peer_Ptr<U>, Peer_Ptr<U> > make_peer_ptrs(Args&&... args);
  };

  template <class T, class... Args>
  std::pair<Peer_Ptr<T>, Peer_Ptr<T> > make_peer_ptrs(Args&&... args)
  {
    auto ret = std::make_pair(Peer_Ptr<T>{}, Peer_Ptr<T>{});

    auto ptr = new T(std::forward<Args>(args)...);

    // The two peers must share the same pointer!
    ret.first.ptr_ = ret.second.ptr_ = new T*(ptr);

    return ret;
  }

  template <class T>
  Peer_Ptr<T>::~Peer_Ptr()
  {
    // This logic only works when there are two peers!! Use a real shared_ptr to make
    // it work with more than two peers, I think.

    // This is also potentially vulnerable to race conditions, where both peers
    // check to see if the pointer has been deallocated and both decide to do
    // the deletion.

    // If our data pointer is valid, we are the first peer to have deallocated.
    if(ptr_ && *ptr_)
    {
      // Delete the data
      delete *ptr_;
      *ptr_ = nullptr;
    }
    else if(ptr_)
    {
      // We don't have a valid data pointer, we are the second peer to have
      // been deallocated, that means delete the shared pointer so we don't
      // leak memory.
      delete ptr_;
    }
  }

  template <class T>
  Peer_Ptr<T>::Peer_Ptr(Peer_Ptr&& rhs)
  {
    this->ptr_ = rhs.ptr_;
    rhs.ptr_ = nullptr;
  }

  template <class T>
  Peer_Ptr<T>& Peer_Ptr<T>::operator=(Peer_Ptr&& rhs)
  {
    this->ptr_ = rhs.ptr_;
    rhs.ptr_ = nullptr;

    return *this;
  }

  template <class T>
  T* Peer_Ptr<T>::operator->()
  {
    return get();
  }

  template <class T>
  T& Peer_Ptr<T>::operator*()
  {
    return *get();
  }

  template <class T>
  T* Peer_Ptr<T>::get() const
  {
    if(ptr_) return *ptr_;
    else return nullptr;
  }
}
