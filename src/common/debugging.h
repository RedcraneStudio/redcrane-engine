/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once

#ifndef NDEBUG

  #define GAME_DEBUGGING_ENABLED

  void game_assert(bool val, char const* const condition,
                   char const* const filename, long line) noexcept;

  // Don't do anything stupid like count on side effects of the condition.
  #define GAME_ASSERT(condition) \
    game_assert((condition), #condition, __FILE__, __LINE__)

#else
  #define GAME_ASSERT(condition) ((void)0)
#endif
