/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file is released under the 3-clause BSD License. The full license text
 * can be found in LICENSE in the top-level directory.
 */
#include <mutex>
namespace redc
{
  struct Null_Lock
  {
    void lock() {}
    void unlock() {}
  };

  struct Mutex_Lock
  {
    void lock() { mut_.lock(); }
    void unlock() { mut_.unlock(); }

    std::mutex mut_;
  };
}
