/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "debugging.h"

#include <cstdlib>
#include "log.h"

void game_assert(bool val, char const* const condition,
               char const* const filename, long line) noexcept
{
  if(!val)
  {
    game::log_e("Assertion '%' failed in %:%", condition, filename, line);
    game::flush_log_full();
    std::abort();
  }
}
