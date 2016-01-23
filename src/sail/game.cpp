/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "game.h"
#include <thread>
#include "../common/log.h"
#include "../net/protocol.h"
#include "../tasks/render.h"
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

  //using namespace redc::literals;

  int start_game(po::variables_map const&) noexcept
  {
    return EXIT_SUCCESS;
  }
  int start_connect(po::variables_map const& vm) noexcept
  {
    // Get details
    auto port = vm["port"].as<uint16_t>();
    auto addr = vm["connect"].as<std::string>();

    // Negotiate connection
    // Just wait for now
    auto connection = net::wait_for_connection(addr, port);

    // Wait for information
    Game game;
    net::wait_for_game_info(connection, game);

    // Set name
    net::set_name(connection, vm["name"].as<std::string>());

    //Input_Task input_task;
    Render_Task render_task(game, "Sail", {1000, 1000}, false, false);

    while(!render_task.should_close())
    {
      //input_task.step();
      //client->step();
      render_task.step();
    }

    net::close_connection(connection);

    return EXIT_SUCCESS;
  }
  int start_dedicated(po::variables_map const&) noexcept
  {
    return EXIT_SUCCESS;
  }
} }

