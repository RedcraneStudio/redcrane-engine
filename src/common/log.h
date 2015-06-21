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
#define GAME_LOG_ATTEMPT_INIT() init_log()
// #define GAME_LOG_ATTEMPT_INIT() ((void*)0)

namespace game
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

  enum class Log_Severity
  {
    Debug, Info, Warning, Error
  };

  void set_log_level(Log_Severity level) noexcept;

  template <class... Args>
  void log(Log_Severity severity, std::string msg, Args&&... args) noexcept
  {
    log(severity, format(msg, 0, "", std::forward<Args>(args)...));
  }
  void log(Log_Severity severity, std::string msg) noexcept;

  void log_e(std::string msg) noexcept;

  template <class... Args>
  void log_e(std::string msg, Args&&... args) noexcept
  {
    log_e(format(msg, 0, "", std::forward<Args>(args)...));
  }

  void log_w(std::string msg) noexcept;

  template <class... Args>
  void log_w(std::string msg, Args&&... args) noexcept
  {
    log_w(format(msg, 0, "", std::forward<Args>(args)...));
  }

  void log_i(std::string msg) noexcept;

  template <class... Args>
  void log_i(std::string msg, Args&&... args) noexcept
  {
    log_i(format(msg, 0, "", std::forward<Args>(args)...));
  }

  void log_d(std::string msg) noexcept;

  template <class... Args>
  void log_d(std::string msg, Args&&... args) noexcept
  {
    log_d(format(msg, 0, "", std::forward<Args>(args)...));
  }
}
