/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include "translate.h"

// We make this a macro so we can just not do it should it become something we
// don't want to be doing for some reason. Performance or IO bottleneck for
// example.
#ifndef REDC_DISABLE_LOGGING
  #define REDC_LOG_ATTEMPT_INIT() init_log()

  #define REDC_LOG_E(...) redc::log_e(__VA_ARGS__)
  #define REDC_LOG_W(...) redc::log_w(__VA_ARGS__)
  #define REDC_LOG_I(...) redc::log_i(__VA_ARGS__)
  #define REDC_LOG_D(...) redc::log_d(__VA_ARGS__)
#else
  #define REDC_LOG_ATTEMPT_INIT() ((void*)0)

  #define REDC_LOG_E ((void*)0)
  #define REDC_LOG_W ((void*)0)
  #define REDC_LOG_I ((void*)0)
  #define REDC_LOG_D ((void*)0)
#endif

namespace redc
{
  struct Scoped_Log_Init
  {
    Scoped_Log_Init() noexcept;
    ~Scoped_Log_Init() noexcept;
  };

  void init_log() noexcept;
  void uninit_log() noexcept;

  void flush_log() noexcept;
  void flush_log_full() noexcept;

  // Higher is more severe!
  enum class Log_Severity : unsigned int
  {
    Debug = 0, Info = 1, Warning = 2, Error = 3
  };

  void set_log_file(std::string filename) noexcept;
  void set_out_log_level(Log_Severity level) noexcept;
  void set_file_log_level(Log_Severity level) noexcept;

  inline void log() {}

  template <class... Args>
  inline void log(Log_Severity severity, std::string msg, Args&&... args) noexcept
  {
    log(severity, format(msg, 0, "", std::forward<Args>(args)...));
  }
  void log(Log_Severity severity, std::string msg) noexcept;

  inline void log_e() {}

  inline void log_e(std::string msg) noexcept
  {
    log(Log_Severity::Error, msg);
  }

  template <class... Args>
  inline void log_e(std::string msg, Args&&... args) noexcept
  {
    log_e(format(msg, 0, "", std::forward<Args>(args)...));
  }

  inline void log_w() {}

  inline void log_w(std::string msg) noexcept
  {
    log(Log_Severity::Warning, msg);
  }

  template <class... Args>
  inline void log_w(std::string msg, Args&&... args) noexcept
  {
    log_w(format(msg, 0, "", std::forward<Args>(args)...));
  }

  inline void log_i() {}

  inline void log_i(std::string msg) noexcept
  {
    log(Log_Severity::Info, msg);
  }

  template <class... Args>
  inline void log_i(std::string msg, Args&&... args) noexcept
  {
    log_i(format(msg, 0, "", std::forward<Args>(args)...));
  }

  inline void log_d() {}

  inline void log_d(std::string msg) noexcept
  {
    log(Log_Severity::Debug, msg);
  }

  template <class... Args>
  inline void log_d(std::string msg, Args&&... args) noexcept
  {
    log_d(format(msg, 0, "", std::forward<Args>(args)...));
  }
}
