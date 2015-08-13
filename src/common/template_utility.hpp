/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <tuple>
#include <type_traits>
namespace game
{
  namespace detail
  {
    struct no {};

    template <class T>
    no operator==(const T& t1, const T& t2);

    template <class T>
    struct has_equality
    {
      static constexpr bool value =
                !std::is_same<decltype(std::declval<T>() == std::declval<T>()),
                              no>::value;
    };
  }

  template <class T>
  struct has_equality : public
               std::integral_constant<bool, detail::has_equality<T>::value> {};

  template <std::size_t I, class T>
    using tuple_element_t = typename std::tuple_element<I, T>::type;

  template <int N, typename... Params>
  struct pack_element
  {
    using type = tuple_element_t<N, std::tuple<Params...> >;
  };

  template <int N, typename... Params>
  using pack_element_t = typename pack_element<N, Params...>::type;

  template <int N, class F, class Tuple_Type, class... Args>
  inline auto
  call_impl(std::enable_if_t<N == std::tuple_size<Tuple_Type>::value, F> f,
            Tuple_Type& tup, Args&&... args) -> decltype(auto)
  {
    return f(std::forward<Args>(args)...);
  }

  template <int N, class F, class Tuple_Type, class... Args>
  inline auto
  call_impl(std::enable_if_t<N < std::tuple_size<Tuple_Type>::value, F> f,
            Tuple_Type& tup, Args&&... args) -> decltype(auto)
  {
    return
    call_impl<N + 1, F, Tuple_Type, Args...>(f, tup,
                                             std::forward<Args>(args)...,
                                             std::get<N>(tup));
  }


  template <class F, class Tuple_Type, class... Args>
  inline auto call(F f, Tuple_Type& tup, Args&&... args) ->decltype(auto)
  {
    return call_impl<0, F, Tuple_Type>(f, tup, std::forward<Args>(args)...);
  }

  template <class, template <class...> class>
  struct wrap_types {};

  template <template <class...> class Wrap, template <class...> class New,
            class... Types>
  struct wrap_types<Wrap<Types...>, New> { using type = New<Types...>; };

  template <class Wrapper, template <class...> class New>
  using wrap_types_t = typename wrap_types<Wrapper, New>::type;

  template <int N, class F, class TupleType, class... Args>
  inline std::enable_if_t<N >= std::tuple_size<TupleType>::value >
  call_foreach(F f, TupleType&& tup, Args&&... args) {}

  template <int N, class F, class TupleType, class... Args>
  inline std::enable_if_t<N < std::tuple_size<TupleType>::value >
  call_foreach(F f, TupleType&& tup, Args&&... args)
  {
    f(std::forward<Args>(args)..., std::get<N>(tup));
    call_foreach<N+1>(f, std::forward<TupleType>(tup),
                      std::forward<Args>(args)...);
  }

  template <class T, class R = void>
  struct enable_if_type
  {
    using type = R;
  };

  namespace detail
  {
    template <typename first, typename... others>
    constexpr bool all_of(first const& f, others const&... o) noexcept
    {
      return f && all_of(o...);
    }
  }

  template <bool... vals>
  struct all_of
    : std::integral_constant<bool, detail::all_of(vals...) > {};
}
