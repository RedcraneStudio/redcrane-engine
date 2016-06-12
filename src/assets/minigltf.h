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

  enum class Buf_View_Target
  {
    Array, Element_Array
  };

  struct Buffer_View
  {
    std::string name;
    uint8_t* base;
    std::size_t size;
    std::size_t offset;
    Buf_View_Target target;
  };

  struct Desc
  {
    boost::optional<fs::path> filepath;
    std::unordered_map<std::string, Buffer> buffers;
    std::unordered_map<std::string, Buffer_View> buf_views;
  };

  Desc load_desc(rapidjson::Value& doc,
                 boost::optional<fs::path> path = boost::none);
  Desc load_desc_file(std::string name);
}
