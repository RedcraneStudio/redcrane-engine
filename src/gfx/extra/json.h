/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "rapidjson/document.h"
#include "glm/glm.hpp"
namespace redc
{
  inline glm::vec3 vec3_from_js_object(rapidjson::Value const& v) noexcept
  {
    return glm::vec3{v["x"].GetDouble(),v["y"].GetDouble(),v["z"].GetDouble()};
  }
  inline glm::vec3 vec3_from_js_array(rapidjson::Value const& v) noexcept
  {
    return glm::vec3{v[0].GetDouble(),v[1].GetDouble(),v[2].GetDouble()};
  }
}
