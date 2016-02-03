/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include <type_traits>
#include <unordered_map>
#include "utility.h"
namespace redc
{
  struct Lang
  {
    std::unordered_map<std::string, std::string> dict;
  };

  void set_lang(Lang const& lang) noexcept;
  Lang get_lang() noexcept;

  inline std::string to_string_helper(std::string str) noexcept { return str; }

  inline std::string to_string_helper(char const* const str) noexcept
  { return str; }
  inline std::string to_string_helper(char* str) noexcept
  { return str; }

  template <class T>
  std::string to_string_helper(T&& v) noexcept { return std::to_string(v); }

  // This is to print integers as binary strings.
  template <class T>
  std::string to_bin_string(T val) noexcept
  {
    std::string ret;

    std::size_t bytes = sizeof(T);
    // Size_t can't necessarily hold the value returned by sizeof * 8 but
    // whateva.
    std::size_t bits = bytes * 8;

    for(std::size_t i = 0; i < bits; ++i)
    {
      // Right to left
      auto bit_where = i % 8;
      auto byte_where = i / 8;

      auto shift = (7 - bit_where) + (bytes - 1 - byte_where);

      ret.append(std::to_string(val & (0xf << shift)));
    }
    return ret;
  }

  inline std::string format(std::string s, int pos, std::string i) noexcept
  {
    // If we have no more to replace just append the original string to
    // whatever we have. This will leave extra percent signs if there are
    // less arguments than percent signs but that's intentional.
    return i + s.substr(pos);
  }

  template <class First, class... Args>
  std::string format(std::string str, int pos, std::string i,
                     First&& first, Args&&... args) noexcept
  {
    // If our position is past the end of the string, just return the
    // intermediate string since it should have been completed. The reason
    // we have to handle it here is because the user may provide extra
    // arguments making the template above this one never be instantiated.
    if(static_cast<int>(str.size()) <= pos) return format(str, pos, i);

    auto c = str[pos];
    if(c == '%')
    {
      // Append the first argument as a string instead of the percent sign.
      i += to_string_helper(std::forward<First>(first));

      // Format further without the 'first' argument since it has been
      // used up.
      return format(str, pos + 1, i, std::forward<Args>(args)...);
    }
    else
    {
      // Pass in the original string, the new intermediate, and the
      // arguments as is, since we haven't used them up yet.
      return format(str, pos + 1, i + c,
                    std::forward<First>(first), std::forward<Args>(args)...);
    }
  }

  template <class... Args>
  std::string format_str(std::string str, Args&&... args) noexcept
  {
    return format(str, 0, "", std::forward<Args>(args)...);
  }

  template <class... Args>
  void write_format_str(std::ostream& o, std::string str,
                        Args&&... args) noexcept
  {
    o << format_str(str, std::forward<Args>(args)...);
  }

  template <class... Args>
  std::string translate(std::string str, Args&&... args) noexcept
  {
    return format(get_lang().dict.at(str), 0, "", std::forward<Args>(args)...);
  }

  template <class... Args>
  std::string t(std::string str, Args&&... args) noexcept
  {
    return translate(str, std::forward<Args>(args)...);
  }

  template <class... Args>
  inline std::string fmt(std::string str, Args&&... args) noexcept
  {
    // TODO remove format_str I probably don't use it.
    return format_str(str, std::forward<Args>(args)...);
  }
}
