/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file is released under the 3-clause BSD License. The full license text
 * can be found in LICENSE in the top-level directory.
 */
namespace redc
{
  template <class E>
  struct Provider_Impl
  {
    // We must be able to poll for some event type.
    virtual bool poll(E& event) = 0;
  };

  //! Represents an event provider which may disappear at some point.
  template <class E>
  struct Provider
  {
    Provider(Peer_Ptr<Provider_Impl<E> > impl) : impl_(std::move(impl)) {}
    ~Provider() = {}

    Provider(Provider const&) = delete;
    Provider& operator=(Provider const&) = delete;

    Provider(Provider&&) = default;
    Provider& operator=(Provider&&) = default;

    bool poll(E& event);
  private:
    Peer_Ptr<Provider_Impl<E> > impl_;
  };

  template <class E>
  bool Provider<E>::poll(E& event)
  {
    // Add a small wrapper around the impl so that it can disappear and no
    // events will be raised. The part where one half can go away is
    // automatically handled by the peer ptr.
    if(impl_)
    {
      return impl_->poll(event);
    }
    else return false;
  }
}
