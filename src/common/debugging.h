/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "log.h"
#ifndef NDEBUG
  #define REDC_DEBUGGING_ENABLED
#endif

// We can't do the noreturn thing unless the compiler supports C++11
#if defined(REDC_DEBUGGING_ENABLED) && __cplusplus >= 201103L
#define REDC_ASSERT_NORETURN [[noreturn]]
#else
#define REDC_ASSERT_NORETURN
#endif

  namespace redc
  {
    [[noreturn]] inline void crash()
    {
      redc::flush_log_full();
      std::abort();
    }

    REDC_ASSERT_NORETURN inline void debug_crash()
    {
      // If we *are* debugging, actually abort and flush the log so we know
      // the messages above will show up.
#ifdef REDC_DEBUGGING_ENABLED
      redc::flush_log_full();
      std::abort();
#endif
    }

    template <class... Msg_Args>
    void assert_fn(bool val, char const* const condition,
                   char const* const filename, int long line,
                   Msg_Args&&... args)
    {
      if(!val)
      {
        log_e(std::forward<Msg_Args>(args)...);
        log_e("Assertion '%' failed in %:%", condition, filename, line);

        debug_crash();
      }
    }
    template <class... Msg_Args>
    REDC_ASSERT_NORETURN
    void unreachable_fn(char const* const fname, int long line,
                        Msg_Args&&... args)
    {
      redc::log_e(std::forward<Msg_Args>(args)...);
      redc::log_e("Unexpected code path reached in %:%", fname, line);

      debug_crash();
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
#define REDC_ASSERT_MSG(condition, ...)                                 \
  ::redc::assert_fn((condition), #condition, __FILE__, __LINE__,        \
                    __VA_ARGS__)

#define REDC_ASSERT_NO_THROW(expr)                                      \
  try { expr; }                                                         \
  catch(std::exception& e)                                              \
  {                                                                     \
    ::redc::unreachable_fn(__FILE__, __LINE__, "Exception thrown by "   \
                           "expression '%':\ntype = %, what() = %",     \
                           #expr, typeid(e).name(), e.what());          \
  }                                                                     \
  catch(...)                                                            \
  {                                                                     \
    ::redc::unreachable_fn(__FILE__, __LINE__, "Unknown exception "     \
                           "thrown by expression: '%'", #expr);         \
  }

#define REDC_UNREACHABLE() ::redc::unreachable_fn(__FILE__, __LINE__)
#define REDC_UNREACHABLE_MSG(...)                                       \
  ::redc::unreachable_fn(__FILE__, __LINE__, __VA_ARGS__)
