/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include <boost/program_options.hpp>
namespace redc { namespace sail
{
  namespace po = boost::program_options;

  po::options_description command_options_desc() noexcept;
  po::variables_map parse_command_options(int argc, char** argv) noexcept;


  int start_game(po::variables_map const& vm) noexcept;
  int start_connect(po::variables_map const& vm) noexcept;
  int start_dedicated(po::variables_map const& vm) noexcept;
} }
