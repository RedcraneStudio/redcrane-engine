/*
 e Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#include <iostream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "common/log.h"

#include "minilua.h"

namespace po = boost::program_options;

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
          ("config-file", po::value<std::string>(), "config file to use")
  ;

  po::options_description server_opt("Networking");
  server_opt.add_options()
          ("name", po::value<std::string>(), "client name")
          ("port", po::value<uint16_t>()->default_value(28222),
           "set port number")
          ("max-peers", po::value<uint16_t>()->default_value(12),
           "set max number of connections")
          ("local-server", "start local server with a client gui")
          ("dedicated-server", "start a dedicated server without a client gui")
          ("advertise-server", "advertise the server to other clients")
          ("connect", po::value<std::string>(), "connect to a server")
  ;

  po::options_description config_opt("Config");
  config_opt.add_options()("render.fov", po::value<float>(),
                           "Field-of-view");

  config_opt.add_options()("render.model_quality", po::value<int>(),
                           "Baseline model quality");
  config_opt.add_options()("render.texture_quality", po::value<int>(),
                           "Baseline texture quality");
  config_opt.add_options()("render.water_quality", po::value<int>(),
                           "Baseline water quality");

  config_opt.add_options()("hud.scale", po::value<float>(), "Hud scale");

  po::options_description desc("Allowed Options");

  desc.add(general_opt).add(server_opt).add(config_opt);

  return desc;
}
int main(int argc, char* argv[])
{
  // At this point we assume we are in a directory where the config can be
  // found, otherwise it should be specified on the command line.

  using namespace redc;

  // Initialize logger
  Scoped_Log_Init log_init_raii_lock{};

  // Parse command line options
  po::variables_map vm;
  auto command_desc = command_options_desc();
  po::store(po::parse_command_line(argc, argv, command_desc), vm);

  if(vm.count("help"))
  {
    std::cerr << command_desc << std::endl;
    return EXIT_SUCCESS;
  }

  namespace fs = boost::filesystem;

  // If the user specified a config file on the command line use that one
  auto cfg_path = fs::path("cfg.ini");
  if(vm.count("config-file"))
  {
    cfg_path = fs::path(vm["config-file"].as<std::string>());
  }
  if(!fs::exists(cfg_path))
  {
    log_e("Config % doesn't exist", cfg_path.native());
    return EXIT_FAILURE;
  }

  // Otherwise we have a configuration file
  po::store(po::parse_config_file<char>(cfg_path.native().c_str(),
                                        command_desc), vm);

  // I don't think we need this but maybe.
  po::notify(vm);

  // Set log severities
  set_out_log_level((Log_Severity) vm["out-log-level"].as<unsigned int>());
  set_file_log_level((Log_Severity) vm["file-log-level"].as<unsigned int>());

  // Log file
  if(vm.count("log-file"))
  {
    set_log_file(vm["log-file"].as<std::string>());
  }

  // Everything we need is in vm now.

  // Initialize LuaJIT
  lua::Scoped_Lua_Init lua_init_raii_lock{};
  auto lua = lua_init_raii_lock.lua;

  // Everything alright?
  if(!lua)
  {
    log_e("Failed to initialize LuaJIT. This is generally caused by a memory"
          "allocation error");
  }
}
