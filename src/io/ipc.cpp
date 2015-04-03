/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "ipc.h"

#include <string>
#include "common.h"

namespace strat { namespace ipc
{
  // Pipe initialization functions.
  Pipe* create_pipe(Process* proc) noexcept
  {
    Pipe* self = new Pipe;
    init_pipe(*self);
    return self;
  }
  void delete_pipe(Pipe* self) noexcept
  {
    uninit_pipe(*self);
    delete self;
  }
  void init_pipe(Pipe& self, Process* proc) noexcept
  {
    self.buf = new std::vector<char>();
    self.proc = proc;
  }

  void uninit_pipe(Pipe& self) noexcept
  {
    delete self.buf;
  }

  // Duplex_Pipe initialization functions.
  Duplex_Pipe* create_duplex_pipe(Process* proc) noexcept
  {
    Duplex_Pipe* self = new Duplex_Pipe;
    init_duplex_pipe(*self, proc);
    return self;
  }
  void delete_duplex_pipe(Duplex_Pipe* self) noexcept
  {
    uninit_duplex_pipe(*self);
    delete self;
  }

  void init_duplex_pipe(Duplex_Pipe& self, Process* proc) noexcept
  {
    // The pipes are already allocated, so we just need to initialize them.
    init_pipe(self.in, proc);
    init_pipe(self.out, proc);
  }
  void uninit_duplex_pipe(Duplex_Pipe& self) noexcept
  {
    uninit_pipe(self.in);
    uninit_pipe(self.out);
  }

  void on_process_exit(uv_process_t* p, int64_t exit, int sig) noexcept
  {
    Process* proc = (Process*) p;

    // Tidy up.
    uv_run(proc->loop, UV_RUN_DEFAULT);

    // Close the process then uninitialize.
    uv_close((uv_handle_t*) proc, NULL);
    proc->running = false;
    delete_process(proc);
  }

  Process* create_process(uv_loop_t* loop, const Spawn_Options& spawn_opt)
  {
    Process* self = new Process;
    self->loop = loop;
    self->running = false;

    // Initialize pipes.
    init_duplex_pipe(self->io, self);
    init_pipe(self->err, self);

    uv_pipe_init(loop, (uv_pipe_t*) &self->io.in, 1);
    uv_pipe_init(loop, (uv_pipe_t*) &self->io.out, 1);
    uv_pipe_init(loop, (uv_pipe_t*) &self->err, 1);

    uv_process_options_t options = {0};

    // Specify executable.
    options.file = spawn_opt.args[0];
    options.args = spawn_opt.args;
    options.cwd = spawn_opt.cwd;

    options.stdio_count = 3;
    uv_stdio_container_t stdio[3];

    // Set up pipes to connect to the child's stdin, stdout, and stderr.
    stdio[0].flags = uv_stdio_flags(UV_CREATE_PIPE | UV_READABLE_PIPE);
    stdio[0].data.stream = (uv_stream_t*) &self->io.out;

    stdio[1].flags = uv_stdio_flags(UV_CREATE_PIPE | UV_WRITABLE_PIPE);
    stdio[1].data.stream = (uv_stream_t*) &self->io.in;

    stdio[2].flags = uv_stdio_flags(UV_CREATE_PIPE | UV_WRITABLE_PIPE);
    stdio[2].data.stream = (uv_stream_t*) &self->err;

    options.stdio = stdio;

    // This callback closes the process handle and then calles delete_process.
    options.exit_cb = on_process_exit;

    std::string proc_name(spawn_opt.args[0]);

    int err = uv_spawn(loop, (uv_process_t*) self, &options);
    if(err)
    {
      throw Spawn_Error{spawn_opt, err};
      delete_process(self);
      return nullptr;
    }

    self->running = true;
    return self;
  }

  void delete_process(Process* self) noexcept
  {
    // Kill the process (if necessary)
    kill_process(self, SIGTERM);

    // Tidy up all pending events.
    uv_run(self->loop, UV_RUN_DEFAULT);

    // Close and uninitialize pipes connected to the child process.
    uv_close((uv_handle_t*) &self->io.in, NULL);
    uninit_pipe(self->io.in);

    uv_close((uv_handle_t*) &self->io.out, NULL);
    uninit_pipe(self->io.out);

    uv_close((uv_handle_t*) &self->err, NULL);
    uninit_pipe(self->err);

    delete self;
  }

  void kill_process(Process* proc, int signum)
  {
    if(!proc->running) return;
    int err = uv_process_kill((uv_process_t*) proc, signum);
    if(err && err != UV_ESRCH)
    {
      throw Kill_Error{proc->proc.pid, signum};
    }
    proc->running = false;
  }

  struct Write_Buf_Req
  {
    uv_write_t req;
    Pipe* pipe;
    on_write_cb after_write;
  };

  void on_write_buffer(uv_write_t* r, int status)
  {
    Write_Buf_Req* req = (Write_Buf_Req*) r;
    if(req->after_write) req->after_write(req->pipe);
    delete req;
  }
  void write_buffer(Pipe* pipe, on_write_cb after_write) noexcept
  {
    uv_buf_t buf = uv_buf_init(&(*pipe->buf)[0], pipe->buf->size());

    Write_Buf_Req* req = new Write_Buf_Req;
    req->pipe = pipe;
    req->after_write = after_write;

    uv_write((uv_write_t*) req, (uv_stream_t*) pipe, &buf, 1, on_write_buffer);
  }

  void collect_lines(uv_stream_t* s, ssize_t nread, const uv_buf_t* buf)
  {
    Pipe* p = (Pipe*) s;
    if(nread == UV_EOF)
    {
      // Call the action as long as the buffer is not empty.
      if(!p->buf->empty()) { p->action_cb(p); }
      uv_read_stop(s);
    }
    else
    {
      for(char* cur = buf->base; cur != buf->base + nread; ++cur)
      {
        if(*cur == '\n')
        {
          if(!p->buf->empty()) { p->action_cb(p); }

          // Clear the buffer.
          p->buf->clear();
        }
        else
        {
          p->buf->push_back(*cur);
        }
      }
    }
    delete[] buf->base;
  }
} }
