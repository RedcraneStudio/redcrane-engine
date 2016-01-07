/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <unordered_map>
#include <queue>
#include "id_gen.hpp"
#include "cache.h"

namespace redc
{
  template <typename T, typename Id = uint8_t>
  struct ID_Map
  {
    using id_type = Id;

    using map_type = std::unordered_map<id_type, T>;

    using iterator = typename map_type::iterator;
    using const_iterator = typename map_type::const_iterator;
    using value_type = typename map_type::value_type;
    using key_type = typename map_type::key_type;
    using size_type = typename map_type::size_type;

    inline ID_Map() noexcept
    {
      this->ids_cache_.gen_func([this](auto)
      {
        auto ids = std::make_unique<std::vector<id_type> >();
        for(auto&& pair : *this)
        {
          ids->push_back(std::get<0>(pair));
        }
        return ids;
      });
    }
    inline id_type insert(T const& obj) noexcept;

    template <class... Args>
    inline id_type emplace(Args&&... args) noexcept;

    inline T const& find(id_type) const;
    inline T& find(id_type);
    inline void set(id_type, T const&);

    inline iterator erase(const_iterator pos);
    inline iterator erase(const_iterator first, const_iterator last);
    inline size_type erase(key_type id);

    inline iterator begin() { return objs_.begin(); }
    inline const_iterator begin() const { return objs_.begin(); }
    inline const_iterator cbegin() const { return objs_.cbegin(); }

    inline iterator end() { return objs_.end(); }
    inline const_iterator end() const { return objs_.end(); }
    inline const_iterator cend() const { return objs_.cend(); }

    inline size_type size() const { return objs_.size(); }

    inline std::vector<id_type> ids() const noexcept;
  private:
    map_type objs_;
    ID_Gen<id_type> id_counter_;

    mutable Cache<std::vector<id_type> > ids_cache_;
  };

  template <class T, class Id>
  inline auto ID_Map<T, Id>::insert(T const& obj) noexcept -> id_type
  {
    id_type id = this->id_counter_.get();
    if(!id) return 0;

    this->objs_.emplace(id, obj);
    this->ids_cache_.invalidate();
    return id;
  }

  template <class T, class Id>
  template <class... Args>
  inline auto ID_Map<T, Id>::emplace(Args&&... args) noexcept -> id_type
  {
    id_type id = this->id_counter_.get();
    if(!id) return 0;

    this->objs_.emplace(id, T{std::forward<Args>(args)...});
    this->ids_cache_.invalidate();
    return id;
  }

  template <class T, class Id>
  inline auto ID_Map<T, Id>::erase(const_iterator pos) -> iterator
  {
    using std::end;
    if(pos != end(this->objs_)) this->id_counter_.remove(pos->first);

    this->ids_cache_.invalidate();
    return this->objs_.erase(pos);
  }
  template <class T, class Id>
  inline auto ID_Map<T, Id>::erase(const_iterator first,
                                   const_iterator last) -> iterator
  {
    const_iterator orig_first = first;
    for(; first != last; ++first) this->id_counter_.remove(first->first);

    this->ids_cache_.invalidate();
    return this->objs_.erase(orig_first, last);
  }
  template <class T, class Id>
  inline auto ID_Map<T, Id>::erase(key_type id) -> size_type
  {
    using std::end;
    if(this->objs_.find(id) != end(this->objs_))
      this->id_counter_.remove(id);

    this->ids_cache_.invalidate();
    return this->objs_.erase(id);
  }

  /*!
   * \brief Returns a const reference to the object with the passed in id.
   * \throws std::out_of_range if the id doesn't correlate with any Object.
   */
  template <class T, class Id>
  inline T const& ID_Map<T, Id>::find(id_type id) const
  {
    return objs_.at(id);
  }

  template <class T, class Id>
  inline T& ID_Map<T, Id>::find(id_type id)
  {
    return this->objs_.at(id);
  }

  template <class T, class Id>
  inline void ID_Map<T, Id>::set(id_type id, T const& obj)
  {
    this->objs_.at(id) = obj;
  }

  template <class T, class Id>
  inline auto ID_Map<T, Id>::ids() const noexcept -> std::vector<id_type>
  {
    return *this->ids_cache_.cache();
  }
}
