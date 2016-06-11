/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <boost/filesystem/path.hpp>
#include "rapidjson/document.h"
namespace redc
{
  namespace fs = boost::filesystem;
  struct Buffer
  {
    std::string name;
    std::vector<uint8_t> buf;
  };
  struct Desc
  {
    boost::optional<fs::path> filepath;
    std::unordered_map<std::string, Buffer> buffers;
  };

  Desc load_desc(rapidjson::Value& doc,
                 boost::optional<fs::path> path = boost::none);
  Desc load_desc_file(std::string name);
}
