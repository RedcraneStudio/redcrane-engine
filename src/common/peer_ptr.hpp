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
    template <class T>
    struct Peer_Ptr_Data
    {
      // Aka payload
      T* ptr;

      std::atomic<unsigned long> lock_count;
      std::atomic<unsigned long> peer_count;

      // When a peer destructs it will signal the locks to uninitialize the
      // payload.
      std::atomic<bool> keep_alive;
    };

    // Attempt to delete the data, based on new values of peer_count, etc.
    template <class T>
    void attempt_reset(Peer_Ptr_Data<T>*& data)
    {
      // We can't do anything if there is a lock on the resource.
      if(data->lock_count.load() == 0)
      {
        // If we aren't compelled to keep the pointer alive, deallocate it.
        if(!data->keep_alive.load())
        {
          // TODO: Support custom deleter!!
          delete data->ptr;
          data->ptr = nullptr;
        }

        // If we are the last peer, we can also deallocate the container.
        if(data->peer_count.load() == 0)
        {
          // The fact that we use delete is coupled to the fact that
          // make_peer_ptrs uses new.
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
    ~Peer_Lock();

    Peer_Lock(Peer_Lock&& ptr);
    Peer_Lock(Peer_Lock const&);

    Peer_Lock& operator=(Peer_Lock&& ptr);
    Peer_Lock& operator=(Peer_Lock const& ptr);

    T* operator->() const;
    T& operator*() const;

    T* get() const;

    void reset();

  private:
    Peer_Lock(detail::Peer_Ptr_Data<T>* data = nullptr);

    detail::Peer_Ptr_Data<T>* data_;

    template <class U>
    friend class Peer_Ptr;
  };

  template <class T>
  Peer_Lock<T>::~Peer_Lock()
  {
    reset();
  }

  template <class T>
  Peer_Lock<T>::Peer_Lock(Peer_Lock&& ptr) : data_(ptr.data_)
  {
    ptr.data_ = nullptr;
  }
  template <class T>
  Peer_Lock<T>::Peer_Lock(Peer_Lock const& rhs) : data_(rhs.data_)
  {
    // If we are given data, inform the container that there is a new lock (us).
    if(data_) ++data_->lock_count;
  }

  template <class T>
  Peer_Lock<T>& Peer_Lock<T>::operator=(Peer_Lock&& rhs)
  {
    this->data_ = rhs.data_;
    rhs.data_ = nullptr;

    return *this;
  }
  template <class T>
  Peer_Lock<T>& Peer_Lock<T>::operator=(Peer_Lock const& rhs)
  {
    // Are we actually locking the same thing?
    if(data_ == rhs.data_) return *this;

    // Nope, reset our data and grab theirs
    reset();
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
  T& Peer_Lock<T>::operator*() const
  {
    return *get();
  }

  template <class T>
  T* Peer_Lock<T>::get() const
  {
    if(data_) return data_->ptr;
    else return nullptr;
  }

  template <class T>
  Peer_Lock<T>::Peer_Lock(detail::Peer_Ptr_Data<T>* data) : data_(data)
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

    // We did all we could do, now it's time to say our farewells.
    data_ = nullptr;
  }

  template <class T>
  struct Peer_Ptr
  {
    ~Peer_Ptr();

    Peer_Ptr(Peer_Ptr&& ptr);
    Peer_Ptr(Peer_Ptr const&) = delete;

    Peer_Ptr& operator=(Peer_Ptr&& ptr);
    Peer_Ptr& operator=(Peer_Ptr const& ptr) = delete;

    T* operator->() const;
    T& operator*() const;

    T* get() const;

    Peer_Lock<T> lock() const;

    void reset();

  private:
    Peer_Ptr(detail::Peer_Ptr_Data<T>* data = nullptr);

    detail::Peer_Ptr_Data<T>* data_;

    template <class U, class... Args>
    friend std::pair<Peer_Ptr<U>, Peer_Ptr<U> > make_peer_ptrs(Args&&... args);
  };

  template <class T>
  Peer_Ptr<T>::~Peer_Ptr()
  {
    reset();
  }

  template <class T>
  Peer_Ptr<T>::Peer_Ptr(Peer_Ptr&& rhs) : data_(rhs.data_)
  {
    rhs.data_ = nullptr;
  }

  template <class T>
  Peer_Ptr<T>& Peer_Ptr<T>::operator=(Peer_Ptr&& rhs)
  {
    this->data_ = rhs.data_;
    rhs.data_ = nullptr;

    return *this;
  }

  template <class T>
  T* Peer_Ptr<T>::operator->() const
  {
    return get();
  }

  template <class T>
  T& Peer_Ptr<T>::operator*() const
  {
    return *get();
  }

  template <class T>
  T* Peer_Ptr<T>::get() const
  {
    if(data_) return data_->ptr;
    else return nullptr;
  }

  template <class T>
  Peer_Lock<T> Peer_Ptr<T>::lock() const
  {
    return Peer_Lock<T>{data_};
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
    data_ = nullptr;
  }

  template <class T>
  Peer_Ptr<T>::Peer_Ptr(detail::Peer_Ptr_Data<T>* data) : data_(data)
  {
    ++data_->peer_count;
  }

  // How client code must construct a Peer_Ptr.

  template <class T, class... Args>
  std::pair<Peer_Ptr<T>, Peer_Ptr<T> > make_peer_ptrs(Args&&... args)
  {
    // Initialize the actual data / payload.
    auto payload = new T(std::forward<Args>(args)...);

    // Allocate a container for our payload.
    auto data = new detail::Peer_Ptr_Data<T>();
    data->ptr = payload;
    data->lock_count = 0;
    data->peer_count = 0;
    data->keep_alive = true;

    // The peer_ptr constructor will increment the peer_count.
    return std::make_pair(Peer_Ptr<T>{data}, Peer_Ptr<T>{data});
  }

}
