/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <unordered_map>
#include <queue>
#include "id_gen.hpp"
#include "cache.h"

#include <boost/optional.hpp>

namespace redc
{
  template <typename T, typename Id>
  struct ID_Map;

  template <typename T, typename Id>
  struct Gen_ID_Vec
  {
    Gen_ID_Vec(ID_Map<T, Id>* map) : map_(map) {}

    using ptr_type = std::unique_ptr<std::vector<Id> >;
    ptr_type operator()(ptr_type);

    ID_Map<T, Id>* map_;
  };

  template <typename T, class Id>
  typename Gen_ID_Vec<T, Id>::ptr_type Gen_ID_Vec<T, Id>::operator()(ptr_type)
  {
    auto ids = std::make_unique<std::vector<Id> >();
    for(auto&& pair : *map_)
    {
      ids->push_back(std::get<0>(pair));
    }
    return ids;
  }

  template <typename T, typename Id = uint16_t>
  struct ID_Map
  {
    using id_type = Id;

    using map_type = std::unordered_map<id_type, T>;

    using iterator = typename map_type::iterator;
    using const_iterator = typename map_type::const_iterator;
    using value_type = typename map_type::value_type;
    using key_type = typename map_type::key_type;
    using size_type = typename map_type::size_type;

    inline ID_Map() noexcept : ids_cache_{this} {}

    inline id_type insert(T const& obj) noexcept;

    template <class... Args>
    inline id_type emplace(Args&&... args) noexcept;

    inline T const& at(id_type) const;
    inline T& at(id_type);

    inline const_iterator find(id_type) const;
    inline iterator find(id_type);
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

    mutable Cache<std::vector<id_type>, Gen_ID_Vec<T, id_type> > ids_cache_;

    // Overall this is a pretty limited mechanism but it really is all we need
    // to implement Active_Map.
    virtual void on_insert(id_type id) {}
    virtual void on_erase(id_type id) {}
  };

  template <class T, class Id>
  inline auto ID_Map<T, Id>::insert(T const& obj) noexcept -> id_type
  {
    id_type id = this->id_counter_.get();
    if(!id) return 0;

    this->objs_.emplace(id, obj);
    this->ids_cache_.invalidate();

    // Do the callback
    on_insert(id);

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

    on_insert(id);

    return id;
  }

  template <class T, class Id>
  inline auto ID_Map<T, Id>::erase(const_iterator pos) -> iterator
  {
    using std::end;
    if(pos != end(this->objs_)) this->id_counter_.remove(pos->first);

    this->ids_cache_.invalidate();

    auto ret = this->objs_.erase(pos);

    // We need to do this only after doing the removal
    on_erase(pos->first);

    return ret;
  }
  template <class T, class Id>
  inline auto ID_Map<T, Id>::erase(const_iterator first,
                                   const_iterator last) -> iterator
  {
    const_iterator orig_first = first;
    for(; first != last; ++first) this->id_counter_.remove(first->first);

    this->ids_cache_.invalidate();
    auto ret = this->objs_.erase(orig_first, last);

    // Remove each id
    for(first = orig_first; first != last; ++first) on_erase(orig_first->first);

    return ret;
  }
  template <class T, class Id>
  inline auto ID_Map<T, Id>::erase(key_type id) -> size_type
  {
    using std::end;
    if(this->objs_.find(id) != end(this->objs_))
      this->id_counter_.remove(id);

    this->ids_cache_.invalidate();
    auto ret = this->objs_.erase(id);

    on_erase(id);

    return ret;
  }

  template <class T, class Id>
  inline T const& ID_Map<T, Id>::at(id_type id) const
  {
    return objs_.at(id);
  }
  template <class T, class Id>
  inline T& ID_Map<T, Id>::at(id_type id)
  {
    return objs_.at(id);
  }

  template <class T, class Id>
  inline auto ID_Map<T, Id>::find(id_type id) const -> const_iterator
  {
    return objs_.find(id);
  }

  template <class T, class Id>
  inline auto ID_Map<T, Id>::find(id_type id) -> iterator
  {
    return objs_.find(id);
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

  template <class T>
  struct Active_Map : public ID_Map<T>
  {
    using id_type = typename ID_Map<T>::id_type;
    // Don't delete through a pointer to base!!

    void active_element(id_type id);
    id_type active_element() const;
  private:
    boost::optional<id_type> active_elem;

    void on_insert(id_type id) override;
    void on_erase(id_type id) override;
  };

  template <class T>
  void Active_Map<T>::active_element(id_type id)
  {
    // Should we check?
    active_elem = id;
  }
  template <class T>
  typename Active_Map<T>::id_type Active_Map<T>::active_element() const
  {
    if(this->size() == 0 || active_elem)
    {
      // Return a bad id?
      // Is zero a bad id?
      return 0;
    }

    // Otherwise this should be set
    return active_elem.value();
  }

  template <class T>
  void Active_Map<T>::on_insert(id_type id)
  {
    // Called after insertion

    // If there is only one element or we don't have an active element for
    // whatever reason.
    if(this->size() == 1 || !active_elem)
    {
      // Our active element is the first one inserted
      active_elem = id;
    }

    // Otherwise, we don't care about it, the user will set it
  }
  template <class T>
  void Active_Map<T>::on_erase(id_type id)
  {
    if(this->size() == 0)
    {
      active_elem = boost::none;
    }
  }

}
