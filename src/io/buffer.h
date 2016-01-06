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

  // For logging
  std::string to_string_helper(buf_t const& buf) noexcept;

  std::string string_from_buf(const buf_t& buf) noexcept;
  std::ostream& operator<<(std::ostream& out, buf_t const& buf) noexcept;

  namespace literals
  {
    std::vector<uchar>
      operator "" _buf(char const* str, std::size_t size) noexcept;
  }
}
