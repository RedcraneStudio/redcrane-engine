/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <type_traits>
#include <functional>
#include <vector>
#include <string>
namespace redc
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

  template <int N, typename... Params>
  struct pack_element
  {
    using type = std::tuple_element_t<N, std::tuple<Params...> >;
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
  namespace detail
  {
    template <bool is_const, typename Dest, typename Source,
              typename F>
    std::vector<Dest> vector_cast(std::conditional_t<is_const,
                                                   const std::vector<Source>&,
                                                   std::vector<Source>&> v,
                                  F f)
    {
      std::vector<Dest> d;

      using Source_Reference =
                          std::conditional_t<is_const, const Source&, Source&>;

      for(Source_Reference s : v)
      {
        d.push_back(f(s));
      }

      return d;
    }
  }

  template <typename Dest, typename Source, typename F>
  inline std::vector<Dest> vector_cast(const std::vector<Source>& v, F f)
  {
    return detail::vector_cast<true, Dest, Source>(v, f);
  }
  template <typename Dest, typename Source>
  inline std::vector<Dest> vector_cast(const std::vector<Source>& v)
  {
    return detail::vector_cast<true, Dest, Source>(v,
    [](const auto& c) { return c; });
  }
  template <typename Dest, typename Source, typename F>
  inline std::vector<Dest> vector_cast(std::vector<Source>& v, F f)
  {
    return detail::vector_cast<false, Dest, Source>(v, f);
  }
  template <typename Dest, typename Source>
  inline std::vector<Dest> vector_cast(std::vector<Source>& v)
  {
    return detail::vector_cast<false, Dest, Source>(v,
    [](auto& c) { return c; });
  }

  /*!
   * \brief Converts a vector of some smart pointer to a vector of just those
   * pointers.
   */
  template <typename Dest, class SP>
  std::vector<Dest> get_data_vector(const std::vector<SP>& v) noexcept
  {
    return vector_cast<Dest>(v, [](const SP& p) { return p.get(); });
  }
}
