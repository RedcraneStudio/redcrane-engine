/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <cstdint>
#include <enet/enet.h>
#include "external_io.h"
#include "../common/result.h"
#include "../common/null_t.h"
namespace redc
{
  namespace net
  {
    enum class Error
    {
      ENet_Init_Failed,
      Host_Init_Failed,
      Connect_Failed
    };

    // RAII wrapper for ENetHost.
    struct Host
    {
      Host() noexcept : host(nullptr) {}
      explicit Host(ENetHost* host) noexcept;
      Host(Host&& oh) noexcept;
      Host& operator=(Host&& oh) noexcept;
      ~Host() noexcept;

      void close() noexcept;

      ENetHost* host;
    };

    Result<Host, Error> make_server_host(uint16_t port,
                                         std::size_t max_peers) noexcept;
    Result<Host, Error> make_client_host() noexcept;

    void connect_with_client(Host& host, std::string h, uint16_t port) noexcept;

    ENetPeer* wait_for_connection(Host& host, uint32_t time) noexcept;
  }

  struct Net_IO : public External_IO
  {
    Net_IO(net::Host& host, ENetPeer* peer, read_cb r_cb = nullptr,
           read_cb e_cb = nullptr) noexcept;

    ~Net_IO() noexcept {}

    // Set reliable packets
    void set_reliable(bool rely) noexcept override;

    // Send a disconnect message to the peer
    void disconnect() noexcept;

    // Write to the peer
    void write(buf_t const& buf) noexcept override;

    // Send out any messages to the peer
    void step() noexcept override;

    // Potentially handles recieved data returning true if it is our peer.
    bool post_recieve(ENetEvent& event) noexcept;

    net::Host* host() const noexcept { return host_; }
    ENetPeer* peer() const noexcept { return peer_; }
  private:
    net::Host* host_;
    ENetPeer* peer_;
    bool send_reliable_ = true;
  };
}
