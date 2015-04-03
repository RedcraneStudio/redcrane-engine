/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include "translate.h"
namespace survive
{
  struct Scoped_Log_Init
  {
    Scoped_Log_Init() noexcept;
    ~Scoped_Log_Init() noexcept;
  };
  void init_log() noexcept;
  void uninit_log() noexcept;
  void flush_log() noexcept;

  enum class Log_Severity
  {
    Debug, Info, Warning, Error
  };

  void set_log_level(Log_Severity level) noexcept;

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
