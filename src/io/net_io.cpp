/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "net_io.h"
#include "../common/log.h"
namespace redc
{
  namespace net
  {
    // Ref-counted wrappers for library-wide init procedures of enet.
    static int init_count = 0;
    int enet_initialize_refct() noexcept
    {
      // Only initialize if init_count is zero
      if(init_count == 0)
      {
        int err = enet_initialize();
        if(err)
        {
          log_e("Error occured initializing enet");
          // Don't incrememnt the refcount.
          return err;
        }
      }

      // We only need to incremement the ref count if everything went well.
      init_count += 1;
      return 0;
    }
    void enet_uninitialize_refct() noexcept
    {
      init_count -= 1;
      if(init_count == 0)
      {
        enet_deinitialize();
      }
    }

    Host::Host(ENetHost* host) noexcept : host(host) {}
    Host::~Host() noexcept
    {
      this->close();
    }
    Host::Host(Host&& oh) noexcept : host(oh.host)
    {
      oh.host = nullptr;
    }
    Host& Host::operator=(Host&& oh) noexcept
    {
      this->close();

      this->host = oh.host;
      oh.host = nullptr;

      return *this;
    }
    void Host::close() noexcept
    {
      if(host)
      {
        enet_host_destroy(host);

        // Just to be safe:
        host = nullptr;

        // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        // @@ make_server adds to the refcount, so we need this here. @@
        // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        enet_uninitialize_refct();
      }
    }

    Result<Host, Error> make_server_host(uint16_t port,
                                         std::size_t max_peers) noexcept
    {
      // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
      // @@ Host::~Host will do the corresponding de-refcount. @@
      // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
      auto init_err = enet_initialize_refct();
      if(init_err) return err(Error::ENet_Init_Failed);

      ENetAddress addr;
      addr.host = ENET_HOST_ANY;
      addr.port = port;

      ENetHost* host = enet_host_create(&addr, max_peers, 1, 0, 0);
      if(!host)
      {
        log_e("Failed to init server host on port % for % max peers", port,
              max_peers);
        return err(Error::Host_Init_Failed);
      }

      return ok(Host(host));
    }
    Result<Host, Error> make_client_host() noexcept
    {
      // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
      // @@ Host::~Host will do the corresponding de-refcount. @@
      // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
      auto init_err = enet_initialize_refct();
      if(init_err) return err(Error::ENet_Init_Failed);

      ENetHost* host = enet_host_create(NULL, 1, 1, 0, 0);
      if(!host)
      {
        log_e("Failed to init client host");
        return err(Error::Host_Init_Failed);
      }

      return ok(Host(host));
    }

    void connect_with_client(Host& host, std::string h, uint16_t port) noexcept
    {
      ENetAddress addr;
      enet_address_set_host(&addr, h.data());
      addr.port = port;

      enet_host_connect(host.host, &addr, 1, 0);
    }

    ENetPeer* wait_for_connection(Host& host, uint32_t time) noexcept
    {
      ENetEvent event;
      if(enet_host_service(host.host, &event, time))
      {
        if(event.type == ENET_EVENT_TYPE_CONNECT)
        {
          // Eeh, this is kind of confusing. If there is no connection no move
          // is done.
          return event.peer;
        }
        else
        {
          return nullptr;
        }
      }
      return nullptr;
    }
  } // namespace net

  Net_IO::Net_IO(net::Host& host, ENetPeer* peer,
                 read_cb r_cb, read_cb e_cb) noexcept
    : External_IO(r_cb, e_cb), host_(&host), peer_(peer) {}

  void Net_IO::set_reliable(bool rely) noexcept
  {
    send_reliable_ = rely;
  }

  void Net_IO::disconnect() noexcept
  {
    // TODO: Flag to decide how to disconnect?
    enet_peer_disconnect_later(peer_, 0);
  }

  void Net_IO::write(buf_t const& buf) noexcept
  {
    ENetPacketFlag flags = (ENetPacketFlag) 0;
    if(send_reliable_)
    {
      flags = ENET_PACKET_FLAG_RELIABLE;
    }

    // Should we buffer this and wait for a call to Net_IO::step?
    ENetPacket* packet = enet_packet_create(buf.data(), buf.size(), flags);
    enet_peer_send(peer_, 0, packet);
  }
  void Net_IO::step() noexcept
  {
    enet_host_flush(host_->host);
  }
  bool Net_IO::post_recieve(ENetEvent& event) noexcept
  {
    // If we recieved data from our client / peer.
    if(event.type == ENET_EVENT_TYPE_RECEIVE && event.peer == peer_)
    {
      // Post it
      buf_t buf(event.packet->data,
                event.packet->data + event.packet->dataLength);
      post(buf);
      enet_packet_destroy(event.packet);
      return true;
    }
    return false;
  }
} // namespace redc
