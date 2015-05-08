/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <iterator>
#include "element.h"
namespace game { namespace ui
{
  namespace detail
  {
    // It's a piece of a path of elements.
    struct Element_Piece
    {
      Element_Piece(Shared_Element e, std::size_t cc = -1) noexcept
        : elem(e), cur_child(cc) {}

      Shared_Element elem;
      std::size_t cur_child;
    };
  }

  struct D_F_Elem_Iter :
    public std::iterator<std::forward_iterator_tag, Element>
  {
    D_F_Elem_Iter(Shared_Element e) noexcept;

    // Null iterator construction.
    D_F_Elem_Iter() noexcept;

    D_F_Elem_Iter(D_F_Elem_Iter const&) noexcept;
    D_F_Elem_Iter(D_F_Elem_Iter&&) noexcept;

    D_F_Elem_Iter& operator=(D_F_Elem_Iter const&) noexcept;
    D_F_Elem_Iter& operator=(D_F_Elem_Iter&&) noexcept;

    bool is_end() const noexcept { return path_.front().elem == nullptr; }

    reference operator*() const noexcept;
    pointer operator->() const noexcept;

    D_F_Elem_Iter& operator++() noexcept;
    D_F_Elem_Iter operator++(int) noexcept;

    bool operator==(D_F_Elem_Iter const& rhs) const noexcept;
  private:
    std::vector<detail::Element_Piece> path_;
  };
} }
