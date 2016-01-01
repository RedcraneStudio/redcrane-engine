/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "element_iterator.h"
namespace redc { namespace ui
{
  D_F_Elem_Iter::D_F_Elem_Iter(Shared_Element e) noexcept
  {
    path_.emplace_back(e);
  }
  D_F_Elem_Iter::D_F_Elem_Iter() noexcept
  {
    path_.emplace_back(nullptr);
  }

  D_F_Elem_Iter::D_F_Elem_Iter(D_F_Elem_Iter const& rhs) noexcept
    : path_(rhs.path_) {}
  D_F_Elem_Iter::D_F_Elem_Iter(D_F_Elem_Iter&& rhs) noexcept
    : path_(std::move(rhs.path_)) {}

  D_F_Elem_Iter& D_F_Elem_Iter::operator=(D_F_Elem_Iter const& rhs) noexcept
  {
    path_ = rhs.path_;

    return *this;
  }
  D_F_Elem_Iter& D_F_Elem_Iter::operator=(D_F_Elem_Iter&& rhs) noexcept
  {
    path_ = std::move(rhs.path_);

    return *this;
  }

  D_F_Elem_Iter::reference D_F_Elem_Iter::operator*() const noexcept
  {
    // Get active element pointer and just dereference it.
    return *operator->();
  }
  D_F_Elem_Iter::pointer D_F_Elem_Iter::operator->() const noexcept
  {
    // Return a nullptr in the end-iterator case.
    if(is_end()) return nullptr;

    return path_.back().elem.get();
  }

  D_F_Elem_Iter& D_F_Elem_Iter::operator++() noexcept
  {
    // Get our path.
    auto& cur = path_.back();

    // If we are currently dealing with a nullptr bail out, the logic of this
    // function should prevent this in general unless we are an end iterator
    // or we become an end iterator.
    if(cur.elem == nullptr) return *this;

    // If we still have children for this element that need exploring.
    if(cur.cur_child + 1 < cur.elem->child_count())
    {
      // Mark the first child as being explored.
      ++cur.cur_child;
      // Add it to our path and note that it hasn't been explored.
      path_.emplace_back(cur.elem->child_at(cur.cur_child), -1);
    }

    // If we are done exploring our current element's children, we can go up
    else if(cur.cur_child + 1 == cur.elem->child_count())
    {
      // If we are the root node, we can't go up, just mark ourselves as done
      // by becoming an end iterator.
      if(path_.size() == 1)
      {
        path_.back().elem = nullptr;
        return *this;
      }

      // Get rid of this one.
      path_.erase(path_.end() - 1);

      // Increment ourselves. This will result in us going to the current
      // elements sibling, or if not that the one above its sibling etc.
      operator++();
    }

    return *this;
  }
  D_F_Elem_Iter D_F_Elem_Iter::operator++(int) noexcept
  {
    auto it = *this;
    ++(*this);
    return it;
  }

  bool D_F_Elem_Iter::operator==(D_F_Elem_Iter const& rhs) const noexcept
  {
    return operator->() == rhs.operator->();
  }
  bool operator!=(D_F_Elem_Iter const& lhs, D_F_Elem_Iter const& rhs) noexcept
  {
    return !(lhs == rhs);
  }
} }
