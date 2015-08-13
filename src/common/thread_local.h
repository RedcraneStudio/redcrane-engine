/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#if defined(__GNUC__) || defined(__MINGW32__) || defined(__clang__)
  #define GAME_THREAD_LOCAL __thread
#elif defined(_MSC_VER)
  #define GAME_THREAD_LOCAL __declspec(thread)
#endif
