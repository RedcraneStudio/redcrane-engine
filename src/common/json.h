/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <functional>
#include <string>
#include <vector>

#include "rapidjson/document.h"
namespace redc
{
  bool load_json(rapidjson::Document& doc, std::string const& filename);

  bool has_json_members(rapidjson::Value const&,
                        std::vector<std::string> const&);

  void if_has_member(rapidjson::Value const&, std::string const&,
                     std::function<void (rapidjson::Value const&)>);
}
