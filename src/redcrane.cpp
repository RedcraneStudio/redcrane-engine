/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file provides the implementation for the engine's C interface,
 * specifically tailored for LuaJIT's FFI facilities.
 */

#include "common/log.h"

#include <cstdint>

#include <boost/program_options.hpp>
#include <boost/any.hpp>
#include "redcrane.hpp"

using namespace redc;

extern "C"
{
  const char* redc_get_config_opt(Redc_Engine* rce, const char* opt)
  {
    if(rce->config.count(opt))
    {
      try
      {
        auto str = boost::any_cast<std::string>(rce->config[opt].value());
        auto data = (char*) malloc((str.size() + 1) * sizeof(char));
        std::memcpy((void*) data, &str[0], str.size());
        data[str.size()] = '\0';
      }
      catch(boost::bad_any_cast& bac)
      {
        log_w("Failed to retrieve config option: '%'", opt);
        auto data = (char*) malloc(sizeof(char));
        *data = '\0';
      }
    }
    else
    {
      log_w("'%' does not exist in config", opt);
      auto data = (char *) malloc(sizeof(char));
      *data = '\0';
    }

  }

  void redc_start_connect(Redc_Engine* rce, const char* ip,
                          uint16_t port)
  {
    if(rce->state != Engine_State::None)
    {
      // Shit, we're already in the middle of something
      log_w("Cannot connect to %:%, already in the middle of something",
            ip, port);
    }

    // We are officially now in client mode.
    rce->state = Engine_State::Client;

    // Queue an event?
    // Nah, for now they will just wait

    // Start a connection
  }

}
