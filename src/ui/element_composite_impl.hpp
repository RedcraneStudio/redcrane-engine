/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "cast.h"
namespace game { namespace ui
{
  template <class T>
  void Element_Composite<T>::push_child(Shared_Element v, T l) noexcept
  {
    children_.push_back({v, l});
  }
  template <class T>
  void Element_Composite<T>::push_child(child_t c) noexcept
  {
    children_.push_back(std::move(c));
  }
  template <class T>
  void Element_Composite<T>::insert_child(typename child_vec_t::iterator i,
                                          Shared_Element v,
                                          T l) noexcept
  {
    children_.insert(i, {v, l});
  }
  template <class T>
  void Element_Composite<T>::
  insert_child(typename child_vec_t::const_iterator i, Shared_Element v,
               T l) noexcept
  {
    children_.insert(i, {v, l});
  }
  template <class T>
  void Element_Composite<T>::insert_child(typename child_vec_t::iterator i,
                                          child_t c) noexcept
  {
    children_.insert(i, std::move(c));
  }
  template <class T>
  void
  Element_Composite<T>::insert_child(typename child_vec_t::const_iterator i,
                                     child_t c) noexcept
  {
    children_.insert(i, std::move(c));
  }

  template <class T>
  void Element_Composite<T>::remove_child(Shared_Element v) noexcept
  {
    using std::begin; using std::end;
    auto find_iter = std::find_if(begin(children_), end(children_),
    [&](auto const& c)
    {
      return c.view == v;
    });
    if(find_iter != end(children_))
    {
      children_.erase(find_iter);
    }
  }
  template <class T>
  void Element_Composite<T>::remove_child(typename child_vec_t::size_type
                                          index) noexcept
  {
    children_.erase(children_.begin() + index);
  }
  template <class T>
  void Element_Composite<T>::remove_child(typename child_vec_t::iterator
                                          iter) noexcept
  {
    children_.erase(iter);
  }
  template <class T>
  void Element_Composite<T>::remove_child(typename child_vec_t::const_iterator
                                          i) noexcept
  {
    children_.erase(i);
  }

  template <class T> Shared_Element
  Element_Composite<T>::find_child_(std::string id, bool r) const noexcept
  {
    for(auto const& child : children_)
    {
      // Check our direct child to see if *it* has the id we need.
      if(child.view->id == id) return child.view;
      // If it doesn't and we are recursively trying to look.
      else if(r)
      {
        // Look in it's children like this. Regular views will always fail
        // to find anything, while view container implementations will
        // check each child and their children as we are doing.
        auto found = child.view->find_child(id, r);
        if(found) return found;
      }
    }
    return nullptr;
  }
  template <class T> bool Element_Composite<T>::
  replace_child_(std::string id, Shared_Element v, bool r) noexcept
  {
    for(auto& child : children_)
    {
      // Check our direct child to see if *it* has the id we need.
      if(child.view->id == id)
      {
        // Replace it's view with the one that needs replacing.
        child.view = v;
        // stop
        return true;
      }
      // If it doesn't and we are recursively trying to look.
      else if(r)
      {
        // Look in it's children like this. Regular views will always fail
        // to replcae anything, while view container implementations will
        // check each child and their children as we are doing.
        if(child.view->replace_child(id, v, r)) return true;
      }
    }
    return false;
  }

  template <class T>
  void Element_Composite<T>::render_(Renderer& r) const noexcept
  {
    for(auto& child : children_)
    {
      child.view->render(r);
    }
  }
} }
