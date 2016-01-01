/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "debugging.h"

#include <cstdlib>
#include "log.h"

namespace redc
{
  void assert_fn(bool val, char const* const condition,
                   char const* const filename, long line) noexcept
  {
    if(!val)
    {
      redc::log_e("Assertion '%' failed in %:%", condition, filename, line);
      redc::flush_log_full();
      std::abort();
    }
  }
}
