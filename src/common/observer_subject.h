/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <vector>
#include <algorithm>
namespace survive
{
  template <class T>
  struct Observer_Subject
  {
    void register_observer(T& obs) noexcept;
    void unregister_observer(T& obs) noexcept;
  protected:
    void sanitize_observers() noexcept;

    std::vector<T*> observers_;
  private:
    virtual void on_observer_add_(T*) const noexcept {}
  };

  template <class T>
  void Observer_Subject<T>::register_observer(T& obs) noexcept
  {
    using std::begin; using std::end;
    auto obj_find = std::find_if(begin(observers_), end(observers_),
    [&](auto const& val)
    {
      return val == &obs;
    });
    // Make sure the observer isn't already in the vector.
    if(obj_find == end(observers_))
    {
      observers_.push_back(&obs);
      on_observer_add_(&obs);
    }
  }
  template <class T>
  void Observer_Subject<T>::unregister_observer(T& obs) noexcept
  {
    // Find the pointer in the vector
    using std::begin; using std::end;
    auto obj_find = std::find_if(begin(observers_), end(observers_),
    [&](auto const& val)
    {
      return val == &obs;
    });
    // If it's valid, remove it.
    if(obj_find != end(observers_))
    {
      observers_.erase(obj_find);
    }
  }

  template <class T>
  void Observer_Subject<T>::sanitize_observers() noexcept
  {
    using std::begin; using std::end;
    auto new_end = std::remove_if(begin(observers_), end(observers_),
    [](auto const& val)
    {
      return val == nullptr;
    });
    observers_.erase(new_end, end(observers_));
  }
}
