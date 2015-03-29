/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <functional>
#include <string>
#include <vector>

#include "rapidjson/document.h"
namespace survive
{
  rapidjson::Document load_json(std::string const& filename) noexcept;

  bool has_json_members(rapidjson::Value const&,
                        std::vector<std::string> const&) noexcept;

  void if_has_member(rapidjson::Value const&, std::string const&,
                     std::function<void (rapidjson::Value const&)>) noexcept;
}
