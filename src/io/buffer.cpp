/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "buffer.h"
namespace redc
{
  bool operator==(std::vector<uchar> const& buf1,
                  std::vector<uchar> const& buf2) noexcept
  {
    if(buf1.size() != buf2.size()) return false;
    return std::equal(buf1.begin(), buf1.end(), buf2.end());
  }

  std::vector<uchar> buf_from_string(const std::string& s) noexcept
  {
    return std::vector<uchar>(s.begin(), s.end());
  }

  namespace literals
  {
    std::vector<uchar>
      operator "" _buf(char const* str, std::size_t size) noexcept
    {
      return std::vector<uchar>(str, str + size);
    }
  }
}

