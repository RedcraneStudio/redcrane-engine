/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#ifndef NDEBUG
  #define REDC_DEBUGGING_ENABLED

  namespace redc
  {
    void assert_fn(bool val, char const* const condition,
                   char const* const filename, long line) noexcept;
  }

  // Don't do anything stupid like count on side effects of the condition.
  #define REDC_ASSERT(condition) \
    ::redc::assert_fn((condition), #condition, __FILE__, __LINE__)

  #define REDC_ASSERT_NO_THROW(expr) \
    try { expr; } \
    catch(std::exception& e) \
    { \
      log_e("% thrown by expression. Crashing", e.what()); \
      ::redc::assert_fn(false, #expr, __FILE__, __LINE__); \
    } \
    catch(...) \
    { \
      log_e("unknown exception thrown by expression. Crashing"); \
      ::redc::assert_fn(false, #expr, __FILE__, __LINE__); \
    } \

#else
  #define REDC_ASSERT(condition) ((void)0)
#endif
