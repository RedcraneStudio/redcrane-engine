/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "game.h"
#include <thread>
#include "server.h"

#include "../io/net_io.h"

#include "../common/log.h"
namespace redc { namespace sail
{
  po::options_description command_options_desc() noexcept
  {
    po::options_description general_opt("General");
    general_opt.add_options()
      ("help", "display help")
      ("out-log-level", po::value<unsigned int>()->default_value(2),
       "set minimum log level to stdout")
      ("log-file", po::value<std::string>(), "set log file")
      ("file-log-level", po::value<unsigned int>()->default_value(0),
       "set minimum log level to file")
    ;

    po::options_description boat_opt("Boat");
    boat_opt.add_options()
      ("hull", po::value<unsigned int>()->default_value(0), "set boat hull")
      ("sail", po::value<unsigned int>()->default_value(0), "set boat sail")
      ("rudder", po::value<unsigned int>()->default_value(0), "set boat rudder")
      ("gun", po::value<unsigned int>()->default_value(0), "set boat gun")
    ;

    po::options_description server_opt("Server");
    server_opt.add_options()
      ("port", po::value<uint16_t>()->default_value(28222), "set port number")
      ("max-peers", po::value<uint16_t>()->default_value(12),
       "set max number of connections")
      ("local-server", "start local server with a client gui")
      ("dedicated-server", "start a dedicated server without a client gui")
      ("advertise-server", "advertise the server to other clients")
      ("connect", po::value<std::string>(), "connect to a server")
    ;

    po::options_description desc("Allowed Options");

    desc.add(general_opt).add(boat_opt).add(server_opt);

    return desc;
  }
  po::variables_map parse_command_options(int argc, char** argv) noexcept
  {
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, command_options_desc()), vm);
    po::notify(vm);

    return vm;
  }
  Server_Mode pick_server_mode(po::variables_map const& vm) noexcept
  {
    bool dedicated = vm.count("dedicated-server");
    bool local = vm.count("local-server");
    bool connect = vm.count("connect");

    int total = dedicated + local + connect;
    if(total > 1)
    {
      return Server_Mode::Bad;
    }

    if(dedicated) return Server_Mode::Dedicated;
    if(connect) return Server_Mode::Connect;
    // By default, start a local server.
    else return Server_Mode::Local;
  }

  using namespace redc::literals;

  int start_game(po::variables_map const& vm) noexcept
  {
    auto port = vm["port"].as<uint16_t>();
    auto max_peers = vm["max-peers"].as<uint16_t>();

    auto host = net::make_server_host(port, max_peers);
    Server server{std::move(*host.ok()), {}};

    Client_Dispatcher client_dispatcher(server);

    bool running = true;
    while(running)
    {
      for(auto& client : server.clients)
      {
        client.second.io->step();
      }

      ENetEvent event;
      while(enet_host_service(server.host.host, &event, 0) != 0)
      {
        auto id = client_dispatcher.try_client_dispatch(event);
        if(id)
        {
          // That was a connection!
          auto& client = server.clients.find(id);
          log_i("Client % (id = %) connected", client.name, id);


          // Add a read-callback so we know what that client gets data.
          client.io->set_read_callback([&client](buf_t const& buf)
          {
            log_i("%: '%'", client.name, buf);
          });
        }
        else if((id = client_dispatcher.check_client_disconnect(event)))
        {
          log_i("Client % disconnected", id);
        }
        else
        {
          // Something else?
          for(auto& client_pair : server.clients)
          {
            auto& client = client_pair.second;
            if(client.io->post_recieve(event))
            {
              // We just handled the event, because it matched the peer of some
              // client.
              // Break from this loop
              break;
            }
          }
        }
      }
    }

    return EXIT_SUCCESS;
  }
  int start_connect(po::variables_map const& vm) noexcept
  {
    auto hostname = vm["connect"].as<std::string>();
    auto port = vm["port"].as<uint16_t>();
    log_i("Joining server '%:%'", hostname, port);

    auto host = std::move(*net::make_client_host().ok());
    net::connect_with_client(host, hostname, port);
    ENetPeer* peer = nullptr;
    while(!peer)
    {
      // Wait in .5 second periods.
      peer = wait_for_connection(host, 500);
    }

    log_i("Got a connection from %!", peer->address.host);

    auto read_cb = [](buf_t const& buf)
    {
      log_i("Server said: %", buf);
    };

    Net_IO net_io{host, peer, read_cb};

    net_io.write("Hello my friend!"_buf);
    net_io.step();

    net_io.disconnect();

    ENetEvent event;
    while(enet_host_service(host.host, &event, 100));

    return EXIT_SUCCESS;
  }
  int start_dedicated(po::variables_map const& vm) noexcept
  {
    log_w("Dedicated server not fully implemented");

    auto port = vm["port"].as<uint16_t>();
    auto max_peers = vm["max-peers"].as<uint16_t>();

    auto host = std::move(*net::make_server_host(port, max_peers).ok());

    ENetPeer* peer = nullptr;
    while(!peer)
    {
      // Wait in .5 second periods.
      peer = wait_for_connection(host, 500);
    }

    log_i("Got a connection from %!", peer->address.host);

    Net_IO net_io{host, peer};

    using namespace redc::literals;

    net_io.write("Hello, my friend!"_buf);
    while(true)
    {
      net_io.step();
    }

    return EXIT_SUCCESS;
  }
} }

