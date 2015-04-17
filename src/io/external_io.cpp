/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "external_io.h"
#include "common.h"
namespace game
{
  void post_pipe_buffer(ipc::Pipe* p) noexcept
  {
    External_IO* io = (External_IO*) p->user_data;
    io->post(*p->buf);
  }
  void post_error_from_buffer(ipc::Pipe* p) noexcept
  {
    External_IO* io = (External_IO*) p->user_data;
    io->post_error(*p->buf);
  }

  Child_Process::Child_Process(ipc::Spawn_Options& opt) noexcept
  {
    uv_loop_init(&loop_);

    process_ = ipc::create_process(&loop_, opt);

    process_->io.in.user_data = this;
    process_->io.in.action_cb = post_pipe_buffer;
    uv_read_start((uv_stream_t*) &process_->io.in, alloc, ipc::collect_lines);

    process_->err.user_data = this;
    process_->err.action_cb = post_error_from_buffer;
    uv_read_start((uv_stream_t*) &process_->err, alloc, ipc::collect_lines);

    // Start the boll rollin'!
    // *process_->io.out.buf = vec_from_string("PpM");
    ipc::write_buffer(&process_->io.out);
  }
  Child_Process::~Child_Process() noexcept
  {
    ipc::delete_process(process_);

    uv_run(&loop_, UV_RUN_DEFAULT);
    uv_loop_close(&loop_);
  }
  void Child_Process::write(std::vector<char> const& buf) noexcept
  {
    step();
    if(!process_->running) return;
    *process_->io.out.buf = buf;
    ipc::write_buffer(&process_->io.out);
  }
  void Child_Process::step() noexcept
  {
    uv_run(&loop_, UV_RUN_NOWAIT);
  }

  void post_net_buffer(net::Pipe* pipe)
  {
    Net_IO* io = (Net_IO*) pipe->user_data;
    io->post(*pipe->out.buf);
  }

  Net_IO::Net_IO(std::string const& bind_ip,
                 uint16_t const bind_port,
                 std::string const& write_ip,
                 uint16_t const write_port) noexcept
  {
    // Initialize the loop
    uv_loop_init(&loop_);
    // Initialize the network pipe.
    net::init_pipe(pipe_, &loop_, bind_ip, bind_port);
    pipe_.user_data = this;
    pipe_.read_cb = post_net_buffer;

    // Initialize the address that will be written to.
    uv_ip4_addr(write_ip.c_str(), write_port, &write_addr_);
  }
  Net_IO::~Net_IO() noexcept
  {
    // Uninitialize the net pipe
    net::uninit_pipe(pipe_);
    // Uninit the loop
    uv_loop_close(&loop_);
  }

  void Net_IO::write(std::vector<char> const& buf) noexcept
  {
    *pipe_.out.buf = buf;
    net::write_buffer(pipe_, (struct sockaddr*) &write_addr_);
  }
  void Net_IO::step() noexcept
  {
    uv_run(&loop_, UV_RUN_NOWAIT);
  }

  Pipe_IO::Pipe_IO() noexcept
  {
    // Construct the counterpart and point it to us.
    cp_.set_owned(new Pipe_IO(*this));
  }

  Pipe_IO& Pipe_IO::counterpart() noexcept
  {
    return *cp_;
  }

  void Pipe_IO::write(std::vector<char> const& buf) noexcept
  {
    // Write to the counterpart's input.
    cp_->input_.push(buf);
  }
  void Pipe_IO::step() noexcept
  {
    step_(true);
  }
  void Pipe_IO::step_(bool recursive) noexcept
  {
    // Read our pending input.
    while(!input_.empty())
    {
      std::vector<char> const& buf = input_.front();
      post(buf);
      input_.pop();
    }

    // Let our counterpart do the same.
    if(recursive)
    {
      cp_->step_(false);
    }
  }
}
