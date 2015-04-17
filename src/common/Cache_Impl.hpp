/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "template_utility.hpp"
namespace game
{
  /*!
   * \brief Move constructor, moves the cache!
   */
  template <typename T, class D, class... Depends>
  Cache_Impl<T, D, Depends...>::Cache_Impl(Cache_Impl&& c) noexcept
                                           : cache_(std::move(c.cache_)),
                                             gen_func_(std::move(c.gen_func_)),
                                             deps_(std::move(c.deps_)){}

  /*!
   * \brief Move assignment operator, moves the cache!
   */
  template <typename T, class D, class... Depends>
  auto Cache_Impl<T, D, Depends...>::operator=(Cache_Impl&& c) noexcept
                                                            -> Cache_Impl&
  {
    this->cache_ = std::move(c.cache_);
    this->gen_func_ = std::move(c.gen_func_);

    this->deps_ = std::move(c.deps_);

    return *this;
  }

  /*!
   * \brief Copies the generation function only.
   */
  template <typename T, class D, class... Depends>
  Cache_Impl<T, D, Depends...>::Cache_Impl(const Cache_Impl& c) noexcept
                                           : gen_func_(c.gen_func_),
                                             deps_(c.deps_){}

  /*!
   * \brief Copies the generation function only.
   */
  template <typename T, class D, class... Depends>
  auto Cache_Impl<T, D, Depends...>::operator=(const Cache_Impl& c) noexcept
                                                                 -> Cache_Impl&
  {
    this->gen_func_ = c.gen_func_;
    this->deps_ = c.deps_;

    return *this;
  }

  /*!
   * \brief Returns the cache, but won't generate it at all.
   *
   * Can stand for Constant-time Cache, Current Cache, or just Const Cache!
   *
   * \returns The current state of the cache currently (no generation is done.)
   */
  template <typename T, class D, class... Depends>
  inline const T* Cache_Impl<T, D, Depends...>::ccache() const noexcept
  {
    return this->cache_.get();
  }

  /*!
   * \brief Returns the cache, generating it if necessary.
   *
   * A generation occurs if the returned pointer will be a nullptr, if it
   * still is a nullptr after the generation than that is what is returned.
   */
  template <typename T, class D, class... Depends>
  inline T* Cache_Impl<T, D, Depends...>::cache()
  {
    if(!this->cache_) this->generate();
    return this->cache_.get();
  }

  /*!
   * \brief Returns a dependency.
   *
   * \returns The Nth element of the dependency tuple.
   *
   * \sa Cache_Impl::cache_
   */
  template <typename T, class D, class... Depends>
  template <std::size_t N>
  inline auto Cache_Impl<T, D, Depends...>::get_dependency() const noexcept ->
                typename std::tuple_element<N, depends_tuple_type>::type const&
  {
    return std::get<N>(this->deps_);
  }

  /*!
   * \brief Returns a non-const reference of a dependency.
   *
   * This function is great for calling a single method on some dependency
   * where you otherwise would need to do a call to get_dependency, then
   * set_dependency. It's also good where a copy won't cut it, for instance
   * when dealing with a cache dependency.
   *
   * \warning Storing the reference and using it later can cause the cache to
   * become out of date without warning. It's best used with discretion.
   */
  template <typename T, class D, class... Depends>
  template <std::size_t N> inline auto
  Cache_Impl<T, D, Depends...>::grab_dependency() noexcept ->
                      typename std::tuple_element<N, depends_tuple_type>::type&
  {
    return std::get<N>(this->deps_);
  }

  template <typename T>
  inline bool maybe_equality(
           const T& t1,
  typename std::enable_if<!has_equality<T>::value, T>::type const& t2) noexcept
  {
    return false;
  }

  template <typename T>
  inline bool maybe_equality(
            const T& t1,
   typename std::enable_if<has_equality<T>::value, T>::type const& t2) noexcept
  {
    return t1 == t2;
  }

  /*!
   * \brief Sets a dependency of the generation possibly invalidating the
   * cache.
   *
   * The cache is invalidated if the passed in dependency value is unequal to
   * the current value.
   *
   * \sa Cache_Impl::cache_
   */
  template <typename T, class D, class... Depends>
  template <std::size_t N>
  inline void Cache_Impl<T, D, Depends...>::set_dependency(typename
           std::tuple_element<N, depends_tuple_type>::type const& dep) noexcept
  {
    if(maybe_equality(dep, std::get<N>(this->deps_))) return;

    std::get<N>(this->deps_) = dep;
    this->invalidate();
  }

  /*!
   * \brief Generates the cache possibly factoring in its previous value.
   *
   * \returns The nullptrness of the cache after the generation.
   *
   * \note This function will always (re)generate the cache.
   */
  template <typename T, class D, class... Depends>
  inline auto Cache_Impl<T, D, Depends...>::generate() -> bool
  {
    this->cache_ = call(this->gen_func_, this->deps_, std::move(this->cache_));
    return static_cast<bool>(this->cache_);
  }

  /*!
   * \brief Invalidates the cache completely.
   *
   * This means it will have to be completely regenerated at some later time.
   */
  template <typename T, class D, class... Depends>
  inline auto Cache_Impl<T, D, Depends...>::invalidate() noexcept -> void
  {
    this->cache_.reset(nullptr);
  }

  /*!
   * \brief Returns the generation function.
   *
   * \returns Cache_Impl::gen_func_.
   */
  template <typename T, class D, class... Depends>
  inline auto
  Cache_Impl<T, D, Depends...>::gen_func() const noexcept -> gen_func_type
  {
    return this->gen_func_;
  }

  /*!
   * \brief Sets the generation function of the cache.
   *
   * \param f The new function. If empty this function is a complete no-op.
   * \post Invalidates the cache if f is nonempty.
   */
  template <typename T, class D, class... Depends>
  inline
  void Cache_Impl<T, D, Depends...>::gen_func(gen_func_type f) noexcept
  {
    // f is empty? Get out!
    if(!f) return;
    this->invalidate();
    this->gen_func_ = f;
  }
}
