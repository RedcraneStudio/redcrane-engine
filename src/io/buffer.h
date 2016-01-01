/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <vector>
#include <string>
namespace redc
{
  using uchar = unsigned char;
  using buf_t = std::vector<uchar>;

  bool operator==(std::vector<uchar> const& b1,
                  std::vector<uchar> const& b2) noexcept;

  std::vector<uchar> buf_from_string(const std::string& s) noexcept;

  namespace literals
  {
    std::vector<uchar>
      operator "" _buf(char const* str, std::size_t size) noexcept;
  }
}
