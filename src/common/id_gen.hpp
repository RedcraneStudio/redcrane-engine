/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <vector>
#include <algorithm>
namespace redc
{
  template <typename id_type>
  struct ID_Gen
  {
    id_type get();
    id_type peek();

    void remove(id_type id);
    bool is_removed(id_type id);

    bool is_valid(id_type id);

    id_type reserved();

  private:
    id_type count_ = 0;

    // We could technically use a set here, but the performance is never going
    // to be better than the vector for the amount of ids that we are going to
    // actually need in practice.
    std::vector<id_type> removed_id_queue_;
  };

  /*!
   * \brief Returns some valid which can be used on a new Object.
   *
   * \returns 0 if there are no ids available.
   */
  template <typename id_type>
  id_type ID_Gen<id_type>::get()
  {
    // If we have ids to use
    if(!removed_id_queue_.empty())
    {
      auto id = removed_id_queue_.back();
      removed_id_queue_.pop_back();
      return id;
    }

    // What do we do on integer overflow?
    id_type ret = ++count_;

    // If there was an overflow, go back one so this happens every time from
    // now on. Also zero is a bad id so it works. Remember ret will still be
    // zero once we decrement count.
    if(ret == 0) --count_;

    return ret;
  }

  template <typename id_type>
  id_type ID_Gen<id_type>::peek()
  {
    if(!removed_id_queue_.empty())
    {
      return removed_id_queue_.front();
    }
    else
    {
      // If this overflows it will be zero, we're good.
      return count_ + 1;
    }
  }

  template <typename id_type>
  void ID_Gen<id_type>::remove(id_type id)
  {
    // This is a bad id, ignore it or we may just go about returning them
    // erroneously.
    if(id == 0) return;

    // The id can't be bigger than count, because count is the last id that was
    // returned. In fact, if we end up overflowing it will always be that one
    // number right before overflow so everything will work out.
    if(id <= count_)
    {
      // Make sure it isn't already in the vector.
      if(!is_removed(id))
      {
        // We don't particularly care when this id is used, but at the moment
        // it will be the next one to be used, ie the result of peek().
        removed_id_queue_.push_back(id);
      }
      // else the id was found, we don't need to remove it again.
    }
  }
  template <class id_type>
  bool ID_Gen<id_type>::is_removed(id_type id)
  {
    auto it = std::find(begin(removed_id_queue_), end(removed_id_queue_), id);
    return it != end(removed_id_queue_);
  }

  template <class id_type>
  bool ID_Gen<id_type>::is_valid(id_type id)
  {
    // A valid id can't be removed and must be below/equal to count_
    return !is_removed(id) &&  id <= count_;
  }

  template <typename id_type>
  id_type ID_Gen<id_type>::reserved()
  {
    return count_ - removed_id_queue_.size();
  }
}
