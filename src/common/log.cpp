/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "log.h"
#include <string>
#include <ctime>
#include <cstring>
#include <uv.h>
namespace game
{
  Scoped_Log_Init::Scoped_Log_Init() noexcept
  {
    init_log();
  }
  Scoped_Log_Init::~Scoped_Log_Init() noexcept
  {
    uninit_log();
  }

  thread_local uv_loop_t* loop_ = nullptr;
  Log_Severity level_ = Log_Severity::Info;

  void init_log() noexcept
  {
    if(loop_) return;

    loop_ = new uv_loop_t;
    uv_loop_init(loop_);
  }
  void uninit_log() noexcept
  {
    if(!loop_) return;

    uv_loop_close(loop_);
    delete loop_;
  }
  void flush_log() noexcept
  {
    uv_run(loop_, UV_RUN_NOWAIT);
  }
  void flush_log_full() noexcept
  {
    uv_run(loop_, UV_RUN_DEFAULT);
  }

  std::string format_time(const std::string& format)
  {
    // Stringify current time.
    std::string time;
    time.resize(25);

    std::time_t t = std::time(NULL);
    std::size_t count = std::strftime(&time[0], time.size(),
                                      format.c_str(), std::localtime(&t));
    if(count == 0)
    {
      time = "badtime";
    }

    time.resize(count);
    return time;
  }

  struct write_req_t
  {
    uv_fs_t req;
    uv_buf_t buf;
  };

  void after_log(uv_fs_t* fs_req)
  {
    write_req_t* req = (write_req_t*) fs_req;

    // Uninitialize actual uv request.
    uv_fs_req_cleanup(&req->req);

    // Deallocate buffer.
    delete[] req->buf.base;

    // Deallocate request subclass.
    delete req;
  }

  void set_log_level(Log_Severity level) noexcept
  {
    level_ = level;
  }

  void log(std::string severity, std::string msg,
           char const* const before, char const* const after) noexcept
  {
    if(!loop_) return;

    // Create the final message.
    std::string time = format_time("%F|%T");
    std::string final_msg = std::string{before} + "(" + time + "): " +
                            severity + ": " + msg + std::string{after} + "\n";

    // Copy to a buffer libuv can use.
    char* msg_data = new char[final_msg.size()];
    std::memcpy(msg_data, final_msg.data(), final_msg.size());

    // Make the request.
    write_req_t* req = new write_req_t;
    req->buf = uv_buf_init(msg_data, final_msg.size());
    uv_fs_write(loop_, &req->req, 1, &req->buf, 1, -1, after_log);
  }
  void log(Log_Severity severity, std::string msg) noexcept
  {
    // This could be implemented differently, for instance with a
    // enum-to-string function + calling the log(string,string) function
    // directly.
    switch(severity)
    {
      case Log_Severity::Error:
        log_e(msg);
        break;
      case Log_Severity::Warning:
        log_w(msg);
        break;
      case Log_Severity::Info:
        log_i(msg);
        break;
      case Log_Severity::Debug:
        log_d(msg);
    }
  }

  constexpr char const* const RESET_C = "\x1b[0m";
  constexpr char const* const ERROR_C = "\x1b[95m";
  constexpr char const* const WARNING_C = "\x1b[91m";
  constexpr char const* const INFO_C = "\x1b[92m";
  constexpr char const* const DEBUG_C = "\x1b[96m";

  void log_e(std::string msg) noexcept
  {
    if(static_cast<int>(level_) <= static_cast<int>(Log_Severity::Error))
      log("error", msg, ERROR_C, RESET_C);
  }
  void log_w(std::string msg) noexcept
  {
    if(static_cast<int>(level_) <= static_cast<int>(Log_Severity::Warning))
      log("warning", msg, WARNING_C, RESET_C);
  }
  void log_i(std::string msg) noexcept
  {
    if(static_cast<int>(level_) <= static_cast<int>(Log_Severity::Info))
      log("info", msg, INFO_C, RESET_C);
  }
  void log_d(std::string msg) noexcept
  {
    if(static_cast<int>(level_) <= static_cast<int>(Log_Severity::Debug))
      log("debug", msg, DEBUG_C, RESET_C);
  }
}
