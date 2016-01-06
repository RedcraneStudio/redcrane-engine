/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "net_io.h"
#include "../common/log.h"
namespace redc { namespace net
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
} }
