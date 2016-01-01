/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <functional>
#include <tuple>
namespace redc
{
  /*!
   * \brief Implements a caching system for arbitrary pointers.
   */
  template <typename T, class D, class... Depends>
  class Cache_Impl
  {
  public:
    /*!
     * \brief Smart pointer type which will manage the cache.
     */
    using ptr_type = std::unique_ptr<T, D>;
    /*!
     * \brief Generation function type.
     */
    using gen_func_type = std::function<ptr_type (ptr_type, Depends&...)>;

    /*!
     * \brief Dependencies tuple type.
     */
    using depends_tuple_type = std::tuple<Depends...>;

    /*!
     * \brief Constructs a cache without any particular generation function.
     *
     * If a cache generation is requested while the generation function is
     * still invalid/empty, an exception is thrown. Change that with
     * Cache_Impl::gen_func(const gen_func_type&).
     */
    Cache_Impl() noexcept : gen_func_() {}
    /*!
     * \brief Constructs a cache with a generation function.
     */
    Cache_Impl(gen_func_type f) noexcept : gen_func_(f) {}

    Cache_Impl(Cache_Impl&&) noexcept;
    Cache_Impl& operator=(Cache_Impl&&) noexcept;

    Cache_Impl(const Cache_Impl&) noexcept;
    Cache_Impl& operator=(const Cache_Impl&) noexcept;

    inline const T* ccache() const noexcept;
    inline T* cache();

    template <std::size_t N> inline auto get_dependency() const noexcept ->
               typename std::tuple_element<N, depends_tuple_type>::type const&;

    template <std::size_t N> inline auto grab_dependency() noexcept ->
               typename std::tuple_element<N, depends_tuple_type>::type&;

    template <std::size_t N> inline void
    set_dependency(typename
              std::tuple_element<N, depends_tuple_type>::type const&) noexcept;

    inline bool generate();
    inline void invalidate() noexcept;

    inline gen_func_type gen_func() const noexcept;
    inline void gen_func(gen_func_type f) noexcept;
  private:

    /*!
     * \brief Smart pointer managing the cache.
     */
    ptr_type cache_ = nullptr;

    /*!
     * \brief Function used to generate the cache.
     */
    gen_func_type gen_func_;

    /*!
     * \brief Tuple of dependencies.
     *
     * This tuple is later expanded into arguments to the gen func.
     */
    depends_tuple_type deps_;
  };

  template <typename T, class... Depends>
  using Cache = Cache_Impl<T, std::default_delete<T>, Depends...>;

  template <typename T, class D, class... Depends>
  using Cache_With_Deleter = Cache_Impl<T, D, Depends...>;
};

#include "cache_impl.hpp"
