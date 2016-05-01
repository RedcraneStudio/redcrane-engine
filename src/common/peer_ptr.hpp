/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <atomic>
namespace redc
{
  // We have a lot of resources that don't need to be kept alive by the engine,
  // but do need to be properly uninitialized before the engine is
  // uninitialized itself. We can achieve this by sharing the ownership of the
  // pointer between peer objects. One of them goes out of scope? Deallocate
  // the data immediately.

  // The above being said, it would also be convenient if we could have
  // external references to the peer that keep it alive. Basically, the peer
  // represents the lowest common denominator of use. When all the shared
  // references (different type of object) get destructed and at least one peer
  // no longer cares about the object, we can finally uninitialize the memory.
  // This makes sense for our engine because we must only keep a resource alive
  // if:
  // - The mod is currently holding a reference to it
  // - Some part of the engine is holding a reference to it
  // However, deallocation must happen if
  // - The engine is about to be uninitialized itself, or
  // - Neither the mod nor any part of the engine is using the resource.

  // This is different from a shared_ptr / weak_ptr relationship because if we
  // store a weak_ptr in the engine and give lua a shared_ptr, lua gets to call
  // the shots, when to uninitialize the resource, which could be after we
  // uninitialize the engine - that doesn't help us.
  // On the other hand, if we give the weak_ptr to lua and give the shared_ptr
  // to the engine, we'll have to manually go through and figure out what
  // resources have been dropped / gc'd by lua anyway. This can be done with
  // some sort of primitive gc step of our own every few frames, but would
  // still complicate the interface of the Engine structure, as well as any
  // associated implementation. Furthermore, I have no idea how many resources
  // are going to be controlled in this way, meaning it would only get worse.
  // This is just my solution / abstraction to this problem. Also, this is not
  // typical use of shared_ptr / weak_ptr so it could get confusing, etc.

  // Our solution still requires a basic gc step to remove all unused peer
  // pointers, but we can probably go a while not worrying about it, or better
  // yet, do it gradually! When allocating a resource, we give one peer pointer
  // to Lua, one peer pointer to the engine. Any internal users of the resource
  // can lock() it to keep it alive for the time being. The locking
  // functionality will not be directly exposed to the lua mod. The only issue
  // will be circular dependencies. I think the trick here is to order the
  // members of Engine such that things only depend on things above them and
  // not the other way around, that way everything will be destructed in the
  // right order. It's easy enough to make a weak lock or just use a vanilla
  // pointer, etc.

  // two scenarios where data
  // *should* be uninitialized: When lua is done with it and when the engine
  // is uninitialized. The two things are the reverse of mutually exclusive -
  // each one

  namespace detail
  {
    struct Deleter_Base
    {
      virtual void unallocate() = 0;
    };

    template <class T>
    struct Default_Deleter : public Deleter_Base
    {
      Default_Deleter(T* ptr) : ptr(ptr) {}

      T* ptr;

      inline void unallocate() override
      {
        delete ptr;
      }
    };

    template <class T, class D>
    struct Deleter_Wrapper : public Deleter_Base
    {
      Deleter_Wrapper(T* ptr, D deleter) : ptr(ptr), deleter(deleter) {}

      T* ptr;
      D deleter;

      inline void unallocate() override
      {
        deleter(ptr);
      }
    };

    struct Peer_Ptr_Data
    {
      std::unique_ptr<Deleter_Base> deleter;

      std::atomic<unsigned long> lock_count;
      std::atomic<unsigned long> peer_count;

      // When a peer destructs it will signal the locks to uninitialize the
      // payload.
      std::atomic<bool> keep_alive;
    };

    // Attempt to delete the data, based on new values of peer_count, etc.
    inline void attempt_reset(Peer_Ptr_Data*& data)
    {
      // We can't do anything if there is a lock on the resource.
      if(data->lock_count.load() == 0)
      {
        // If we aren't compelled to keep the pointer alive, deallocate it.
        if(!data->keep_alive.load())
        {
          if(data->deleter) data->deleter->unallocate();
          data->deleter.release();
        }

        // If we are the last peer, we can also deallocate the container.
        if(data->peer_count.load() == 0)
        {
          // TODO: Support allocators for this, not necessarily C++ allocators
          // just so we don't go allocating / deallocating willy-nilly.

          // Delete the container.
          delete data;
          data = nullptr;
        }
      }
    }
  }

  // This is a standard ref-counted interface to the peer pointer, these will
  // keep the pointer alive so use them sparingly.

  // It's possible that we may have to deallocate either of the pointers here.
  template <class T>
  struct Peer_Lock
  {
    Peer_Lock() : data_(nullptr) {}
    ~Peer_Lock();

    template <class U>
    Peer_Lock(Peer_Lock<U>&& ptr);

    template <class U>
    Peer_Lock(Peer_Lock<U> const&);

    template <class U>
    Peer_Lock& operator=(Peer_Lock<U>&& ptr);

    template <class U>
    Peer_Lock& operator=(Peer_Lock<U> const& ptr);

    T* operator->() const;
    typename std::add_lvalue_reference<T>::type operator*() const;

    T* get() const;

    void reset();

  private:
    template <class U>
    Peer_Lock(U* ptr, detail::Peer_Ptr_Data* data);

    T* ptr_;
    detail::Peer_Ptr_Data* data_;

    template <class U>
    friend class Peer_Ptr;
  };

  template <class T>
  Peer_Lock<T>::~Peer_Lock()
  {
    reset();
  }

  template <class T>
  template <class U>
  Peer_Lock<T>::Peer_Lock(Peer_Lock<U>&& ptr)
    : ptr_(ptr.ptr_), data_(ptr.data_)
  {
    // There is no net increase or decrease in the amount of locks so just take
    // data directly from the other lock.
    ptr.ptr_ = nullptr;
    ptr.data_ = nullptr;
  }
  template <class T>
  template <class U>
  Peer_Lock<T>::Peer_Lock(Peer_Lock<U> const& rhs)
    : ptr_(rhs.ptr_), data_(rhs.data_)
  {
    // If we are given data, inform the container that there is a new lock (us).
    if(data_) ++data_->lock_count;
  }

  template <class T>
  template <class U>
  Peer_Lock<T>& Peer_Lock<T>::operator=(Peer_Lock<U>&& rhs)
  {
    // U* must be compatible with T* otherwise there will be a compile error.
    this->ptr_  = rhs.ptr_;
    this->data_ = rhs.data_;

    rhs.ptr_ = nullptr;
    rhs.data_ = nullptr;

    return *this;
  }
  template <class T>
  template <class U>
  Peer_Lock<T>& Peer_Lock<T>::operator=(Peer_Lock<U> const& rhs)
  {
    // Are we actually locking the same thing?
    if(data_ == rhs.data_) return *this;

    // Nope, reset our data and grab theirs
    reset();
    ptr_ = rhs.ptr_;
    data_ = rhs.data_;

    // One more lock
    if(data_) ++data_->lock_count;
    return *this;
  }

  template <class T>
  T* Peer_Lock<T>::operator->() const
  {
    return get();
  }
  template <class T>
  typename std::add_lvalue_reference<T>::type Peer_Lock<T>::operator*() const
  {
    return *get();
  }

  template <class T>
  T* Peer_Lock<T>::get() const
  {
    if(ptr_ && data_ && data_->deleter.get()) return ptr_;
    else return nullptr;
  }

  template <class T>
  template <class U>
  Peer_Lock<T>::Peer_Lock(U* ptr, detail::Peer_Ptr_Data* data)
    : ptr_(ptr), data_(data)
  {
    ++data->lock_count;
  }

  template <class T>
  void Peer_Lock<T>::reset()
  {
    // No data?
    if(!data_) return; // No problem!

    // One less lock
    --data_->lock_count;

    // Do any cleanup that is necessary at this point.
    attempt_reset(data_);

    // We did all we could do, now it's time to bid our farewells.
    ptr_ = nullptr;
    data_ = nullptr;
  }

  // Does not support array types or allocators
  template <class T>
  struct Peer_Ptr
  {
    Peer_Ptr() : ptr_(nullptr), data_(nullptr) {}

    template <class U>
    explicit Peer_Ptr(U* pointer);

    template <class U, class D>
    Peer_Ptr(U* pointer, D deleter);

    // D must be CopyConstructable because we aren't able to move it from the
    // unique_ptr.
    template <class U, class D>
    Peer_Ptr(std::unique_ptr<U, D> ptr);

    // See above
    template <class U, class D>
    Peer_Ptr& operator=(std::unique_ptr<U, D> ptr);

    ~Peer_Ptr();

    template <class U>
    Peer_Ptr(Peer_Ptr<U>&& ptr);

    template <class U>
    Peer_Ptr& operator=(Peer_Ptr<U>&& ptr);

    Peer_Ptr(Peer_Ptr const&) = delete;
    Peer_Ptr& operator=(Peer_Ptr const& ptr) = delete;

    T* operator->() const;
    typename std::add_lvalue_reference<T>::type operator*() const;

    T* get() const;
    explicit operator bool() const;

    Peer_Ptr<T> peer() const;
    Peer_Lock<T> lock() const;

    std::size_t peers() const;
    std::size_t locks() const;

    void reset();

  private:
    template <class U>
    Peer_Ptr(U* ptr, detail::Peer_Ptr_Data* data);

    T* ptr_;
    detail::Peer_Ptr_Data* data_;

    template <class U>
    friend class Peer_Ptr;
  };

  template <class T>
  template <class U>
  Peer_Ptr<T>::Peer_Ptr(U* pointer)
    : ptr_(pointer), data_(new detail::Peer_Ptr_Data)
  {
    // Use the default deleter (it just calls delete).
    // TODO: Use delete[] if U is an array type.
    data_->deleter = std::make_unique<detail::Default_Deleter<U> >(pointer);

    data_->lock_count = 0;
    data_->keep_alive = true;
    data_->peer_count = 1;
  }

  template <class T>
  template <class U, class D>
  Peer_Ptr<T>::Peer_Ptr(U* pointer, D del)
    : ptr_(pointer), data_(new detail::Peer_Ptr_Data)
  {
    data_->deleter =
            std::make_unique<detail::Deleter_Wrapper<U, D> >(pointer, del);

    data_->lock_count = 0;
    data_->keep_alive = true;
    data_->peer_count = 1;
  }
  // Unique pointer construction, the engine will probably make new peers later
  // with peer().
  template <class T>
  template <class U, class D>
  Peer_Ptr<T>::Peer_Ptr(std::unique_ptr<U, D> uniq_ptr)
    : ptr_(nullptr), data_(nullptr)
  {
    *this = std::move(uniq_ptr);
  }
  template <class T>
  template <class U, class D>
  Peer_Ptr<T>& Peer_Ptr<T>::operator=(std::unique_ptr<U, D> uniq_ptr)
  {
    reset();

    // Don't worry about it if this is a nullptr.
    if(!uniq_ptr) return *this;

    data_ = new detail::Peer_Ptr_Data;

    // Get a copy of the deleter first so the unique_ptr doesn't do anything
    // clever with it.
    data_->deleter =
      std::make_unique<detail::Deleter_Wrapper<U, D> >(uniq_ptr.get(),
                                                       uniq_ptr.get_deleter());

    this->ptr_ = uniq_ptr.release();

    data_->lock_count = 0;
    data_->keep_alive = true;

    // Us!
    data_->peer_count = 1;

    return *this;
  }

  template <class T>
  Peer_Ptr<T>::~Peer_Ptr()
  {
    reset();
  }

  template <class T>
  template <class U>
  Peer_Ptr<T>::Peer_Ptr(Peer_Ptr<U>&& rhs) : ptr_(rhs.ptr_), data_(rhs.data_)
  {
    // No net increase in number of peers, and no reason to queue a pointer
    // dealloc.
    rhs.ptr_ = nullptr;
    rhs.data_ = nullptr;
  }

  template <class T>
  template <class U>
  Peer_Ptr<T>& Peer_Ptr<T>::operator=(Peer_Ptr<U>&& rhs)
  {
    this->ptr_ = rhs.ptr_;
    this->data_ = rhs.data_;

    rhs.ptr_ = nullptr;
    rhs.data_ = nullptr;

    return *this;
  }

  template <class T>
  T* Peer_Ptr<T>::operator->() const
  {
    return get();
  }

  template <class T>
  typename std::add_lvalue_reference<T>::type Peer_Ptr<T>::operator*() const
  {
    return *get();
  }

  template <class T>
  T* Peer_Ptr<T>::get() const
  {
    // If we have a pointer, data, and a data deleter all valid it means the
    // pointer points to something meaningful.
    if(ptr_ && data_ && data_->deleter.get()) return ptr_;
    else return nullptr;
  }

  template <class T>
  Peer_Ptr<T>::operator bool() const
  {
    return get() != nullptr;
  }

  template <class T>
  Peer_Lock<T> Peer_Ptr<T>::lock() const
  {
    return Peer_Lock<T>{ptr_, data_};
  }
  template <class T>
  Peer_Ptr<T> Peer_Ptr<T>::peer() const
  {
    // Be careful! We are making yet another in for the data to be delete'd out
    // from under us!
    return Peer_Ptr{ptr_, data_};
  }

  template <class T>
  std::size_t Peer_Ptr<T>::peers() const
  {
    if(!data_) return 0;
    return data_->peer_count.load();
  }
  template <class T>
  std::size_t Peer_Ptr<T>::locks() const
  {
    if(!data_) return 0;
    return data_->lock_count.load();
  }

  template <class T>
  void Peer_Ptr<T>::reset()
  {
    // If we don't have any data, fo'get about it
    if(!data_) return;

    // At least one less peer (us) no longer active.
    --data_->peer_count;

    // Since we're a peer, the data can be deallocated as soon as possible.
    data_->keep_alive = false;

    // Attempt to reclaim the data if possible at this point.
    attempt_reset(data_);

    // Byebye data
    ptr_ = nullptr;
    data_ = nullptr;
  }

  template <class T>
  template <class U>
  Peer_Ptr<T>::Peer_Ptr(U* ptr, detail::Peer_Ptr_Data* data)
    : ptr_(ptr), data_(data)
  {
    if(data_) ++data_->peer_count;
  }

  template <class T, class... Args>
  Peer_Ptr<T> make_peer_ptr(Args&&... args)
  {
    auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
    return Peer_Ptr<T>{std::move(ptr)};
  }

  // How client code must construct a Peer_Ptr.
  template <class T, unsigned int N, class... Args>
  std::enable_if_t<N == (sizeof...(Args) + 1), std::array<Peer_Ptr<T>, N> >
  make_peer_array_impl(Peer_Ptr<T>&& ptr, Args&&... args)
  {
    // We have all peer pointers we need, move them into a new array.
    return std::array<Peer_Ptr<T>, N>{std::move(ptr), std::forward<Args>(args)...};
  }

  template <class T, unsigned int N, class... Args>
  std::enable_if_t<N != (sizeof...(Args) + 1), std::array<Peer_Ptr<T>, N> >
  make_peer_array_impl(Peer_Ptr<T>&& ptr, Args&&... args)
  {
    // We can't use fill because we do want many (independent) instances of a
    // peer and we can't do a copy.

    // Add a peer, we are not there yet!
    return make_peer_array_impl<T, N>(std::move(ptr),
                                      std::forward<Args>(args)...,
                                      ptr.peer());
  }


  template <class T, unsigned int N, class... Args>
  std::array<Peer_Ptr<T>, N> make_peer_array(Args&&... args)
  {
    // Our args are for the payload

    // Initialize the actual data / payload.
    auto peer = make_peer_ptr<T>(std::forward<Args>(args)...);
    return make_peer_array_impl<T, N>(std::move(peer));
  }

  template <class T, class... Args>
  std::pair<Peer_Ptr<T>, Peer_Ptr<T> > make_peer_pair(Args&&... args)
  {
    auto arr = make_peer_array<T, 2>(std::forward<Args>(args)...);
    return std::make_pair(std::move(arr[0]), std::move(arr[1]));
  }
}
