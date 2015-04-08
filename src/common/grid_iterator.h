/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
namespace strat
{
  namespace detail
  {
    template <class T>
    struct Grid_Pt
    {
      T row;
      T col;
    };

    template <class T1, class T2>
    bool operator==(Grid_Pt<T1> const& lhs, Grid_Pt<T2> const& rhs) noexcept
    {
      return lhs.row == rhs.row && lhs.col == rhs.col;
    }

    template <class T>
    struct Grid_Iterator
    {
      Grid_Iterator() noexcept;
      Grid_Iterator(T max_rows, T max_cols, T cur_row, T cur_col) noexcept;

      Grid_Pt<T> const& operator*() const noexcept;
      Grid_Pt<T> const* operator->() const noexcept;

      Grid_Iterator& operator++() noexcept;
      Grid_Iterator operator++(int) noexcept;
      Grid_Iterator& operator--() noexcept;
      Grid_Iterator operator--(int) noexcept;

      bool operator==(Grid_Iterator<T> const& rhs) const noexcept;
    private:
      T max_rows_ = 0;
      T max_cols_ = 0;

      Grid_Pt<T> pt_;

      void increment_() noexcept;
      void deincrement_() noexcept;
    };

    // Past-the-end iterator construction.
    template <class T>
    Grid_Iterator<T>::Grid_Iterator() noexcept
      : max_rows_(0), max_cols_(0), pt_{0, 0} {}

    template <class T>
    Grid_Iterator<T>::Grid_Iterator(T max_rows, T max_cols,
                                    T cur_row, T cur_col) noexcept
      : max_rows_(max_rows), max_cols_(max_cols), pt_{cur_row, cur_col} {}

    template <class T>
    Grid_Pt<T> const& Grid_Iterator<T>::operator*() const noexcept
    {
      return pt_;
    }

    template <class T>
    Grid_Pt<T> const* Grid_Iterator<T>::operator->() const noexcept
    {
      return &pt_;
    }

    template <class T>
    Grid_Iterator<T>& Grid_Iterator<T>::operator++() noexcept
    {
      increment_();
      return *this;
    }
    template <class T>
    Grid_Iterator<T> Grid_Iterator<T>::operator++(int) noexcept
    {
      auto this_old = *this;
      increment_();
      return this_old;
    }
    template <class T>
    Grid_Iterator<T>& Grid_Iterator<T>::operator--() noexcept
    {
      deincrement_();
      return *this;
    }
    template <class T>
    Grid_Iterator<T> Grid_Iterator<T>::operator--(int) noexcept
    {
      auto this_old = *this;
      deincrement_();
      return this_old;
    }

    template <class T>
    bool Grid_Iterator<T>::
    operator==(Grid_Iterator<T> const& rhs) const noexcept
    {
      return max_rows_ == rhs.max_rows_ && max_cols_ == rhs.max_cols_ &&
             pt_ == rhs.pt_;
    }

    template <class T>
    void Grid_Iterator<T>::increment_() noexcept
    {
      if(++pt_.col == max_cols_)
      {
        pt_.col = 0;
        if(++pt_.row == max_rows_)
        {
          // Reset to past the end.
          *this = Grid_Iterator<T>{};
        }
      }
    }
    template <class T>
    void Grid_Iterator<T>::deincrement_() noexcept
    {
      if(--pt_.col == -1)
      {
        pt_.col = 0;
        if(--pt_.row == -1)
        {
          // Reset to past the end.
          *this = Grid_Iterator<T>{};
        }
      }
    }

    template <class T>
    struct Grid_Iterator_Gen
    {
      Grid_Iterator_Gen(T rows, T cols) noexcept : rows_(rows), cols_(cols) {}

      Grid_Iterator<T> begin() const noexcept;
      Grid_Iterator<T> cbegin() const noexcept { return begin(); }
      Grid_Iterator<T> end() const noexcept;
      Grid_Iterator<T> cend() const noexcept { return end(); }
    private:
      T rows_;
      T cols_;
    };

    template <class T>
    Grid_Iterator<T> Grid_Iterator_Gen<T>::begin() const noexcept
    {
      return Grid_Iterator<T>{rows_, cols_, 0, 0};
    }

    template <class T>
    Grid_Iterator<T> Grid_Iterator_Gen<T>::end() const noexcept
    {
      return Grid_Iterator<T>{};
    }
  }

  template <class T>
  detail::Grid_Iterator_Gen<T> make_grid_iterator(T rows, T cols) noexcept
  {
    return detail::Grid_Iterator_Gen<T>{rows, cols};
  }
}
