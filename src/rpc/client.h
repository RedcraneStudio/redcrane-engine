/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../common/task.h"
#include "../io/net_io.h"
#include "dispatch.h"
#include "msgpack_interface.h"
#include "network_client.h"
#include "method_userdata.h"
namespace redc
{
  template <class T>
  struct Client_Task : public Task
  {
    using user_t = User_Type<T>;

    Client_Task(T& data, std::vector<rpc::method_t> methods, std::string host,
                uint16_t port) noexcept;
    void step() noexcept override;

    // Currently the user data is always this
    //void* method_userdata;
  private:
    T* game_;
    std::vector<rpc::method_t> methods_;

    net::Host host_;
    std::unique_ptr<Network_Client> us_;

    bool connected_ = false;
  };

  template <class T>
  Client_Task<T>::Client_Task(T& data, std::vector<rpc::method_t> methods,
                              std::string host, uint16_t port) noexcept
    : game_(&data), methods_(std::move(methods))
  {
    host_ = std::move(*net::make_client_host().ok());
    // Initiate a connection
    connect_with_client(host_, host, port);
  }

  template <class T>
  void Client_Task<T>::step() noexcept
  {
    // Do the io
    if(us_) us_->io->step();

    // Do the enet
    ENetEvent event;
    while(enet_host_service(host_.host, &event, 0))
    {
      if(event.type == ENET_EVENT_TYPE_CONNECT)
      {
        // TODO: Time out if it's been too long.
        connected_ = true;

        // Find the connected peer.
        auto peer = event.peer;

        auto io = make_maybe_owned<Net_IO>(host_, peer);
        us_ = std::make_unique<Network_Client>(std::move(io),
            std::make_unique<rpc::Msgpack_Interface>(ref_mo(io)));
      }
      else if(event.type == ENET_EVENT_TYPE_RECEIVE && us_)
      {
        us_->io->post_recieve(event);
      }
    }

    // Do the dispatch
    rpc::Request req;
    while(us_->plugin.poll_request(req))
    {
      user_t info;
      info.client = us_.get();
      info.game = game_;
      info.userdata = this;

      auto response = dispatch_request(req, methods_, &info);

      if(response)
      {
        us_->plugin.post_request(*response);
      }
    }
  }
}
