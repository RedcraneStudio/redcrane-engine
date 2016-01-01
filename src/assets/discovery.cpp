/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "discovery.h"

#include "../common/log.h"

#include <boost/filesystem.hpp>
namespace redc { namespace assets
{
  namespace fs = boost::filesystem;

  // Return a path equivalent to the second argument relative to the first.
  fs::path make_relative(fs::path base, fs::path path)
  {
    base = fs::absolute(base);
    path = fs::absolute(path);

    auto base_iter = fs::path::const_iterator{base.begin()};
    auto path_iter = fs::path::const_iterator{path.begin()};

    using std::begin; using std::end;

    // Basically discard the common base between both parameters.
    for(; base_iter != end(base) && path_iter != end(path) &&
          *base_iter == *path_iter;
          ++base_iter, ++path_iter)
    {}

    fs::path ret;

    // Navigate backwards for every extra nested directory we are in currently.
    // Remember: Currently == base parameter.
    // TODO: Find a more descriptive name for the base parameter.
    for(; base_iter != end(base); ++base_iter)
    {
      if(*base_iter != ".")
      {
        ret /= "..";
      }
    }

    // The path ret now represents the path to the common ancestor between the
    // two paths. Simply append the second argument to this intermediate value
    // to get our relative path in terms of the first argument.
    for(; path_iter != end(path); ++path_iter)
    {
      ret /= *path_iter;
    }
    return ret;
  }

  auto discover(std::string subdir) noexcept -> std::vector<std::string>
  {
    auto assets = std::vector<std::string>{};

    auto asset_dir = fs::system_complete(subdir);

    if(fs::is_directory(asset_dir))
    {
      auto iter = fs::recursive_directory_iterator{asset_dir};
      auto end = fs::recursive_directory_iterator{};
      for(; iter != end; ++iter)
      {
        if(fs::is_regular_file(*iter))
        {
          // We have a file that we can presumably load.
          // Store the relative path.
          auto path = fs::absolute(iter->path());
          path = make_relative(fs::path{subdir}, path);
          assets.push_back(path.native());
          log_d("Found: " + assets.back());
        }
      }
    }
    else
    {
      log_e("Asset dir provided (" + asset_dir.native() +
            ") not a directory");
    }

    return assets;
  }
} }
