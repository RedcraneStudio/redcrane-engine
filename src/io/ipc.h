/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <uv.h>
#include <vector>

namespace strat { namespace ipc
{
  struct Process;
  struct Pipe
  {
    uv_pipe_t pipe;
    std::vector<char>* buf;
    Process* proc;
    void (*action_cb)(Pipe* p);
    void* user_data;
  };

  Pipe* create_pipe(Process* = nullptr) noexcept;
  void delete_pipe(Pipe*) noexcept;
  void init_pipe(Pipe& self, Process* proc = nullptr) noexcept;
  void uninit_pipe(Pipe& self) noexcept;

  struct Duplex_Pipe
  {
    Pipe in;
    Pipe out;
  };

  Duplex_Pipe* create_duplex_pipe(Process* = nullptr) noexcept;
  void delete_duplex_pipe(Duplex_Pipe* self) noexcept;
  void init_duplex_pipe(Duplex_Pipe& self, Process* = nullptr) noexcept;
  void uninit_duplex_pipe(Duplex_Pipe& self) noexcept;

  using on_write_cb = void (*)(Pipe*);
  void write_buffer(Pipe* pipe, on_write_cb after_write = nullptr) noexcept;

  struct Kill_Error
  {
    int pid;
    int sig;
  };

  struct Spawn_Options
  {
    char** args;
    const char* cwd;
  };

  struct Spawn_Error
  {
    Spawn_Options spawn_options;
    int err;
  };

  struct Process
  {
    uv_process_t proc;
    Duplex_Pipe io;
    Pipe err;
    uv_loop_t* loop;
    bool running;
  };

  Process* create_process(uv_loop_t* loop, const Spawn_Options& spawn_opt);
  void delete_process(Process*) noexcept;
  void kill_process(Process*, int signum);

  void collect_lines(uv_stream_t* s, ssize_t nread, const uv_buf_t* buf);
} };
