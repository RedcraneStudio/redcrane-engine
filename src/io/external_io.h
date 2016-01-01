/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <uv.h>
#include <string>
#include <functional>
#include <queue>
#include "../common/maybe_owned.hpp"
#include "ipc.h"
#include "net.h"
namespace redc
{
  /*!  \brief Manages and connects client code to some arbitrary io source.
   */
  struct External_IO
  {
    using read_cb = std::function<void (buf_t const&)>;

    External_IO(read_cb r_cb = nullptr, read_cb e_cb = nullptr) noexcept
                : read_cb_(r_cb), err_cb_(e_cb) {}

    virtual ~External_IO() noexcept {}

    inline void set_read_callback(read_cb cb) noexcept;
    inline void set_error_callback(read_cb cb) noexcept;
    inline void post(buf_t const& buf) noexcept;
    inline void post_error(buf_t const& err) noexcept;

    virtual void write(buf_t const& buf) noexcept = 0;
    virtual void step() noexcept = 0;
  private:
    read_cb read_cb_;
    read_cb err_cb_;
  };

  inline void External_IO::set_read_callback(read_cb cb) noexcept
  {
    read_cb_ = cb;
  }
  inline void External_IO::set_error_callback(read_cb cb) noexcept
  {
    err_cb_ = cb;
  }
  inline void External_IO::post(buf_t const& buf) noexcept
  {
    read_cb_(buf);
  }
  inline void External_IO::post_error(buf_t const& buf) noexcept
  {
    err_cb_(buf);
  }

  struct Child_Process : public External_IO
  {
    Child_Process(ipc::Spawn_Options&) noexcept;
    ~Child_Process() noexcept;

    void write(buf_t const& buf) noexcept override;
    void step() noexcept override;
  private:
    ipc::Process* process_;
    uv_loop_t loop_;
  };

  struct Net_IO : public External_IO
  {
    Net_IO(std::string const& bind_ip, uint16_t const bind_port,
           std::string const& write_ip, uint16_t const write_port) noexcept;
    ~Net_IO() noexcept;

    void write(buf_t const& buf) noexcept override;
    void step() noexcept override;
  private:
    struct sockaddr_in write_addr_;
    net::Pipe pipe_;
    uv_loop_t loop_;
  };

  struct Pipe_IO : public External_IO
  {
    // Client mode. The counterpart is owned.
    Pipe_IO() noexcept;

    // Get a reference to the counterpart, this function should always return
    // the same reference to the other. Making stuff like:
    // > this->counterpart().counterpart().counterpart().counterpart();
    // work.
    Pipe_IO& counterpart() noexcept;

    // Queue a write to the counterpart.
    void write(buf_t const& buf) noexcept override;

    // Read from the counterpart and send queued buffers.
    void step() noexcept override;
  private:
    void step_(bool recursive) noexcept;

    // Counterpart mode. Counter part is pointed to, not owned.
    Pipe_IO(Pipe_IO& cp) noexcept;

    // Represents a Pipe_IO object that is maybe owned.
    Maybe_Owned<Pipe_IO> cp_;

    std::queue<buf_t> input_;
  };
}
