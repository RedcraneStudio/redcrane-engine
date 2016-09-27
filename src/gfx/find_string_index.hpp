/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#include <vector>
#include <algorithm>
#include "../common/debugging.h"
namespace redc { namespace gfx
{
  template <class... Args>
  std::size_t find_string_index(std::vector<std::string> strs,
                                std::string str,
                                Args&&... msg_args)
  {
    // Search in reverse, so that newer things with identical names will be
    // chosen first.
    auto find_res = std::find(strs.rbegin(), strs.rend(), str);
    REDC_ASSERT_MSG(find_res != strs.rend(), std::forward<Args>(msg_args)...);
    return strs.size() - 1 - (find_res - strs.rbegin());
  }

} }
