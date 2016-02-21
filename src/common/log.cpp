/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "log.h"
#include <string>
#include <ctime>
#include <cstring>
#include <uv.h>
#include "thread_local.h"
namespace redc
{
  Scoped_Log_Init::Scoped_Log_Init() noexcept
  {
    init_log();
  }
  Scoped_Log_Init::~Scoped_Log_Init() noexcept
  {
    flush_log_full();
    uninit_log();
  }

  REDC_THREAD_LOCAL uv_loop_t* loop_ = nullptr;
  Log_Severity out_level_ = Log_Severity::Debug;
  Log_Severity file_level_ = Log_Severity::Debug;
  bool good_file_ = false;
  uv_file file_;

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

  void set_out_log_level(Log_Severity level) noexcept
  {
    // TODO: Add a mutex or something so we don't get that 1-in-1000000 data
    // race. TODO On second thought if this ever comes up in practice buy a
    // lotto ticket!
    out_level_ = level;
  }
  void set_file_log_level(Log_Severity level) noexcept
  {
    file_level_ = level;
  }
  void set_log_file(std::string fn) noexcept
  {
    // This only attempts to initialize loop_, but won't do anything if we call
    // it multiple times.
    init_log();

    uv_fs_t fs_req;
    auto err = uv_fs_open(loop_, &fs_req, fn.c_str(),
                          O_WRONLY | O_APPEND | O_CREAT,
                          S_IRUSR | S_IWUSR, NULL);
    // I still don't really get the usage of uv_fs_open in synchroneous mode.
    // It seems like the return value is the result in fs_res but also an error
    // code if it's negative.
    if(err < 0)
    {
      log_e("Error opening log file: %", uv_strerror(err));
      good_file_ = false;
    }
    else
    {
      file_ = fs_req.result;
      good_file_ = true;
    }
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

  std::string severity_string(Log_Severity severity) noexcept
  {
    switch(severity)
    {
      case Log_Severity::Debug:
        return "debug: ";
      case Log_Severity::Info:
        return "info: ";
      case Log_Severity::Warning:
        return "warning: ";
      case Log_Severity::Error:
        return "error: ";
      default:
        return "";
    }
  }

  constexpr char const* const RESET_C = "\x1b[0m";
  constexpr char const* const ERROR_C = "\x1b[95m";
  constexpr char const* const WARNING_C = "\x1b[91m";
  constexpr char const* const INFO_C = "\x1b[92m";
  constexpr char const* const DEBUG_C = "\x1b[96m";

  std::string severity_color(Log_Severity severity) noexcept
  {
    switch(severity)
    {
      case Log_Severity::Debug:
        return DEBUG_C;
      case Log_Severity::Info:
        return INFO_C;
      case Log_Severity::Warning:
        return WARNING_C;
      case Log_Severity::Error:
        return ERROR_C;
      default:
        //return RESET_C;
        return "";
    }
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

  void log_out(char* msg, std::size_t size) noexcept
  {
    if(!loop_)
    {
      delete[] msg;
      return;
    }

    // Make the request.
    write_req_t* req = new write_req_t;
    req->buf = uv_buf_init(msg, size);
    uv_fs_write(loop_, &req->req, 1, &req->buf, 1, -1, after_log);
  }
  void log_file(char* msg, std::size_t size) noexcept
  {
    // Bad file descriptor or bad loop
    if(!loop_ || !good_file_)
    {
      delete[] msg;
      return;
    }

    write_req_t* req = new write_req_t;
    req->buf = uv_buf_init(msg, size);
    uv_fs_write(loop_, &req->req, file_, &req->buf, 1, -1, after_log);
  }
  void log(Log_Severity severity, std::string msg) noexcept
  {
    // Create the final message.
    std::string time = format_time("%F|%T");

    std::string msg_file = "("+time+"): "+severity_string(severity)+msg+"\n";
    // If the severity of this message is greater than the output minimum.
    if((unsigned int) severity >= (unsigned int) file_level_)
    {
      char* msg_buf = new char[msg_file.size()];
      std::memcpy(msg_buf, msg_file.data(), msg_file.size());
      log_file(msg_buf, msg_file.size());
    }

    if((unsigned int) severity >= (unsigned int) out_level_)
    {
      std::string msg_term = severity_color(severity) + msg_file + RESET_C;
      char* msg_buf = new char[msg_term.size()];
      std::memcpy(msg_buf, msg_term.data(), msg_term.size());
      log_out(msg_buf, msg_term.size());
    }
  }
}
