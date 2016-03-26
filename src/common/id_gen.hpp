/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <queue>
namespace redc
{
  template <typename id_type>
  struct ID_Gen
  {
    using queue_type = std::queue<id_type>;

    id_type count = 0;
    queue_type removed_id_queue;

    inline id_type get();
    inline void remove(id_type id);
  };

  /*!
   * \brief Returns some valid which can be used on a new Object.
   *
   * \returns 0 if there are no ids available.
   */
  template <typename id_type>
  id_type ID_Gen<id_type>::get()
  {
    if(!removed_id_queue.empty())
    {
      id_type id = 0;
      id = removed_id_queue.front();
      removed_id_queue.pop();
      return id;
    }

    // What do we do on integer overflow?
    if(++this->count == 0)
    {
      // If we subtract one from it, this case will continue forever, the only
      // way to get ids back is of course to remove them.
      --this->count;

      // Return zero, signifying a bad id
      return 0;
    }

    return this->count;
  }

  template <typename id_type>
  void ID_Gen<id_type>::remove(id_type id)
  {
    this->removed_id_queue.push(id);
  }

}
