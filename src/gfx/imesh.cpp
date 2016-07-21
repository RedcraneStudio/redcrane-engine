/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "imesh.h"
#include <set>
#include <functional>
#include <algorithm>
namespace redc
{
  struct Buf_Search
  {
    Buf_Search(IMesh::buf_t buf) noexcept : buf_(buf) {}
    bool operator()(std::tuple<IMesh::buf_t, unsigned int> const& tup)
    {
      return std::get<0>(tup) == buf_;
    }
  private:
    IMesh::buf_t buf_;
  };

  unsigned int IMesh::get_num_allocated_buffers() noexcept
  {
    return bufs_.size();
  }
  IMesh::buf_t IMesh::get_buffer(unsigned int index) noexcept
  {
    return std::get<0>(bufs_[index]);
  }
  unsigned int IMesh::get_buffer_size(buf_t buf) noexcept
  {
    auto find_iter = std::find_if(bufs_.begin(), bufs_.end(), Buf_Search{buf});
    if(find_iter != bufs_.end()) return std::get<1>(*find_iter);
    return 0;
  }

  void IMesh::push_buffer_(buf_t buf, unsigned int bytes) noexcept
  {
    bufs_.emplace_back(buf, bytes);
  }
  void IMesh::erase_buffer_(buf_t buf) noexcept
  {
    auto find_iter = std::find_if(bufs_.begin(), bufs_.end(), Buf_Search{buf});
    if(find_iter != bufs_.end()) bufs_.erase(find_iter);
  }
  void IMesh::set_buffer_size_(buf_t buf, unsigned int bytes) noexcept
  {
    auto find_iter = std::find_if(bufs_.begin(), bufs_.end(), Buf_Search{buf});
    if(find_iter != bufs_.end()) std::get<1>(*find_iter) = bytes;
  }
}
