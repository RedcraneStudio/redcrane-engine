/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "log.h"
#ifndef NDEBUG
  #define REDC_DEBUGGING_ENABLED
#endif

  namespace redc
  {
    [[noreturn]] inline void crash()
    {
      redc::flush_log_full();
      std::abort();
    }

    template <class... Msg_Args>
    void assert_fn(bool val, char const* const condition,
                   char const* const filename, int long line,
                   Msg_Args&&... args)
    {
      if(!val)
      {
        redc::log_e(std::forward<Msg_Args>(args)...);
        redc::log_e("Assertion '%' failed in %:%", condition, filename, line);

        // If we *are* debugging, actually abort and flush the log so we know
        // the messages above will show up.
#ifdef REDC_DEBUGGING_ENABLED
        redc::flush_log_full();
        std::abort();
#endif
      }
    }
  }

// Don't do anything stupid like count on side effects of the condition.
#define REDC_ASSERT(condition) \
  ::redc::assert_fn((condition), #condition, __FILE__, __LINE__)

/*
 * This passes everything but the first argument verbatim to the log formatting
 * function. The cool thing about doing it this way is that we use macros to
 * stringify the condition as well as provided an evaluation of that
 * problematic expression which is probably necessary. For example when
 * crashing due to a bad filename, it is very important to provide that
 * filename in the message.
 */
#define REDC_ASSERT_MSG(condition, ...) \
  ::redc::assert_fn((condition), #condition, __FILE__, __LINE__, \
                    __VA_ARGS__)

#define REDC_ASSERT_NO_THROW(expr) \
  try { expr; } \
  catch(std::exception& e) \
  { \
    ::redc::log_e("% thrown by expression. Crashing", e.what()); \
    ::redc::assert_fn(false, #expr, __FILE__, __LINE__); \
  } \
  catch(...) \
  { \
    ::redc::log_e("unknown exception thrown by expression. Crashing"); \
    ::redc::assert_fn(false, #expr, __FILE__, __LINE__); \
  }
