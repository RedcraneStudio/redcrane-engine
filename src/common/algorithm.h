/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <functional>
namespace game
{
  template <class T, class Sort = std::less<T> >
  struct shared_content_less
  {
    explicit shared_content_less(Sort sort = Sort()) noexcept : sort_(sort) {}

    using ptr_type = std::shared_ptr<T>;
    bool operator()(ptr_type const& p1, ptr_type const& p2) noexcept;

  private:
    Sort sort_;
  };

  template <class T, class Sort>
  bool shared_content_less<T, Sort>::operator()(ptr_type const& p1,
                                                ptr_type const& p2) noexcept
  {
    return sort_(*p1.get(), *p2.get());
  }

  template <class T, class Sort = std::less<T> >
  struct unique_content_less
  {
    explicit unique_content_less(Sort sort = Sort()) noexcept : sort_(sort) {}

    using ptr_type = std::unique_ptr<T>;
    bool operator()(ptr_type const& p1, ptr_type const& p2) noexcept;

  private:
    Sort sort_;
  };

  template <class T, class Sort>
  bool unique_content_less<T, Sort>::operator()(ptr_type const& p1,
                                                ptr_type const& p2) noexcept
  {
    return sort_(*p1.get(), *p2.get());
  }

  template <class Container, class T, class Less = std::less<T> >
  void sorted_insert(Container& c, T&& t, Less cfn = Less()) noexcept
  {
    using std::begin; using std::end;

    auto iter = begin(c);
    for(; iter != end(c); ++iter)
    {
      if(!cfn(*iter, t))
      {
        break;
      }
    }

    c.insert(iter, std::forward<T>(t));
  }
}
