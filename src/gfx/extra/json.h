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
  inline bool load_js_vec3(rapidjson::Value const& v, glm::vec3& vec,
                           std::string* err)
  {
    if(v.IsArray())
    {
      vec = vec3_from_js_array(v);
      return true;
    }
    else if(v.IsObject())
    {
      vec = vec3_from_js_object(v);
      return true;
    }
    else
    {
      if(err) (*err) = "Invalid JSON; expected Vec3 (object or array)";
      return false;
    }
  }
}
