/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "game.h"
#include <thread>
#include "../common/log.h"
#include "../net/protocol.h"
#include "../tasks/render.h"
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
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

    po::options_description server_opt("Networking");
    server_opt.add_options()
      ("name", po::value<std::string>(), "client name")
      ("port", po::value<uint16_t>()->default_value(28222), "set port number")
      ("max-peers", po::value<uint16_t>()->default_value(12),
       "set max number of connections")
      ("local-server", "start local server with a client gui")
      ("dedicated-server", "start a dedicated server without a client gui")
      ("advertise-server", "advertise the server to other clients")
      ("connect", po::value<std::string>(), "connect to a server")
    ;

    po::options_description config_opt("Config");
    config_opt.add_options()("render.fov", po::value<float>(), "FOV");

    po::options_description desc("Allowed Options");

    desc.add(general_opt).add(boat_opt).add(server_opt).add(config_opt);

    return desc;
  }
  po::variables_map parse_command_options(int argc, char** argv) noexcept
  {
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, command_options_desc()), vm);
    po::store(po::parse_config_file<char>("assets/cfg.ini", command_options_desc()), vm);
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

  //using namespace redc::literals;

  int start_game(po::variables_map const& vm) noexcept
  {
    Game game;
    Render_Task render(game, vm, "Sail", Vec<int>{1000,1000}, false, false);
    while(!render.should_close())
    {
      render.step();
    }
    return EXIT_SUCCESS;
  }

  template <class T>
  double time_since_start(T before) noexcept
  {
    T now = std::chrono::high_resolution_clock::now();
    using sec_t = std::chrono::duration<double, std::chrono::seconds::period>;
    return sec_t(now - before).count();
  }

  int start_connect(po::variables_map const& vm) noexcept
  {
    auto start_time = std::chrono::high_resolution_clock::now();

    // Get details
    auto port = vm["port"].as<uint16_t>();
    auto addr = vm["connect"].as<std::string>();

    auto client = net::Client{};
    // Initialize necessary fields
    client.ctx.client_version = 0x01;
    enet_address_set_host(&client.ctx.server_addr, addr.c_str());
    client.ctx.server_addr.port = port;

    //Input_Task input_task;
    //Render_Task render_task(game, vm, "Sail", {1000, 1000}, false, false);

    //while(!render_task.should_close())

    // Start connecting we need to do this first so ctx.host is initialized.
    step_client(client.ctx, nullptr);

    int ret_code = EXIT_SUCCESS;
    bool running = true;
    while(running)
    {
      if(client.ctx.state == net::Client_State::Connecting &&
         time_since_start(start_time) > 5.0)
      {
        // We've waited five seconds. Fuck it
        log_e("Failed to connect to %:%", addr, port);
        running = false;
        ret_code = EXIT_FAILURE;
        break;
      }

      ENetEvent event;
      while(enet_host_service(client.ctx.host.host, &event, 0))
      {
        auto res = step_client(client.ctx, &event);

        if(res.context_changed)
        {
          switch(client.ctx.state)
          {
            case net::Client_State::Connecting:
              log_e("Failed to connect to %:%!", addr, port);
              ret_code = EXIT_FAILURE;
              running = false;
              break;
            case net::Client_State::Sending_Loadouts:
              // Send loadouts
              client.ctx.inventory.loadouts.push_back({0,0,0,{0,1}});
              log_i("Sending default loadout");
              break;
            case net::Client_State::Waiting_For_Inventory_Confirmation:
              if(!client.ctx.inventory_okay)
              {
                // Shit
                log_e("Bad inventory");
                ret_code = EXIT_FAILURE;
                running = false;
              }
              break;
            case net::Client_State::Sending_Team:
              // Always join the first team.
              client.ctx.team = client.ctx.client_init_packet.teams[0].id;
              log_i("Picking first time");
              break;
            case net::Client_State::Playing:
              // Do playing things
              log_i("All done connecting, starting game.");
              break;
            default:
              break;
          }
        }

        if(!res.event_handled)
        {
          // Handle it otherwise
        }
      }
    }

    return ret_code;
  }
  int start_dedicated(po::variables_map const& vm) noexcept
  {
    // Get details
    //auto port = vm["port"].as<uint16_t>();
    //auto max_peers = vm["max-peers"].as<uint16_t>();

    //auto server = net::make_server(port, max_peers);

    //Game game;

    return EXIT_SUCCESS;
  }
} }
