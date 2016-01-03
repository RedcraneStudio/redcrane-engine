/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <type_traits>
#include <boost/variant.hpp>
namespace redc
{
  template <class T>
  struct Ok_Wrap
  {
    T obj;
  };

  template <class T>
  struct Err_Wrap
  {
    T obj;
  };

  template <class T>
  inline Ok_Wrap<T> ok(T const& t) noexcept
  {
    return {t};
  }
  template <class T>
  inline Ok_Wrap<T> ok(T&& t) noexcept
  {
    return {std::move(t)};
  }

  template <class T>
  inline Err_Wrap<T> err(T const& t) noexcept
  {
    return {t};
  }
  template <class T>
  inline Err_Wrap<T> err(T&& t) noexcept
  {
    return {std::move(t)};
  }

  template <class Ok_T, class Err_T>
  struct Result
  {
    boost::variant<Ok_Wrap<Ok_T>, Err_Wrap<Err_T> > res;

    template <class T>
    Result(Ok_Wrap<T> const& t) noexcept : res(t) {}

    template <class T>
    Result(Ok_Wrap<T>&& t) noexcept : res(std::move(t)) {}

    template <class T>
    Result(Err_Wrap<T> const& t) noexcept : res(t) {}

    template <class T>
    Result(Err_Wrap<T>&& t) noexcept : res(std::move(t)) {}

    template <class T>
    Result& operator=(Ok_Wrap<T> const& t) noexcept
    {
      res = t;
      return *this;
    }

    template <class T>
    Result& operator=(Err_Wrap<T> const& t) noexcept
    {
      res = t;
      return *this;
    }

    Ok_T const* ok() const noexcept;
    Err_T const* err() const noexcept;
  };

  template <class Ok_T, class Err_T>
  Ok_T const* Result<Ok_T, Err_T>::ok() const noexcept
  {
    if(res.which() == 0)
    {
      return &boost::get<Ok_Wrap<Ok_T> >(res).obj;
    }
    return nullptr;
  }
  template <class Ok_T, class Err_T>
  Err_T const* Result<Ok_T, Err_T>::err() const noexcept
  {
    if(res.which() == 1)
    {
      return &boost::get<Err_Wrap<Err_T> >(res).obj;
    }
    return nullptr;
  }
}
