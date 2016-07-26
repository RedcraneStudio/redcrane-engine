/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#include <src/common/debugging.h>
#include "load_dir.h"

#ifndef REDC_INSTALL_PREFIX
#define REDC_INSTALL_PREFIX
#warning "Compiling without an install prefix; Engine may fail to find asset directory"
#endif

#ifndef REDC_SHARE_DIR
#define REDC_SHARE_DIR "Red_Crane_Engine"
#warning "Compiling without a share directory, default is Red_Crane_Engine"
#endif

namespace redc { namespace assets
{
  using boost::filesystem::path;
  path share_path()
  {
    // Share path is not where assets is

    // Check a few paths to see if they exist:
    std::vector<path> potential_paths {
            // This is the case in the build directory, we want this to be the
            // first one to check for, otherwise we may accidentally use
            // installed assets when we are testing new ones in the build dir.
            path("../assets"),
            // This may be the case installing via steam if we get to that
            // point.
            path("../" REDC_SHARE_DIR),
            // Search the installed prefix, in all likelihood this will resolve
            // to one of the two paths below it but in the case we install
            // someplace weird use this.
            path(REDC_INSTALL_PREFIX "/share/" REDC_SHARE_DIR),
            path("/usr/local/share/" REDC_SHARE_DIR),
            path("/usr/share/" REDC_SHARE_DIR)
    };

    // Check each one and return the top one!
    for(auto const& pathname : potential_paths)
    {
      // Specifically, lets check for cfg.ini in each one, it may redirect us
      // somewhere else so its the only thing we can rely on.
      auto cfg_path = pathname / path("cfg.ini");

      std::string d_msg =
              format_str("Checking in '%' for config: ", pathname.string());

      if(exists(cfg_path))
      {
        d_msg += "Found";
        log_d(d_msg);
        return pathname;
      }
      else
      {
        d_msg += "Not found";
        log_d(d_msg);
      }
    }

    // Return the last one in the vector, just because it is the most likely
    // case. That being said if it doesn't exist it will probably fail. Maybe
    // find a way to signal this error, it's not necessarily a logic bug, more
    // of an installation bug. Actually, I've convinced myself to crash
    // regardless.
    REDC_UNREACHABLE_MSG("Bad installation; could not find assets directory");
  }
} }
