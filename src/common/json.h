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

  template <class... Args>
  void if_has_member(rapidjson::Value const& val, std::string const& mem,
                     std::function<void (rapidjson::Value const&, Args&&...)> fn,
                     Args&&... args)
  {
    if(val.HasMember(mem.c_str()))
    {
      fn(val[mem.c_str()], std::forward<Args>(args)...);
    }
  }
}
