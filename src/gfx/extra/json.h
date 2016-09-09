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
  inline glm::vec3 vec3_from_js_object(picojson::value::object const& v) noexcept
  {
    return glm::vec3(v.at("x").get<double>(),
                     v.at("y").get<double>(),
                     v.at("z").get<double>());
  }
  inline glm::vec3 vec3_from_js_array(picojson::value::array const& v) noexcept
  {
    return glm::vec3{v[0].get<double>(),v[1].get<double>(),v[2].get<double>()};
  }
  inline bool load_js_vec3(picojson::value const& v, glm::vec3& vec,
                           std::string* err)
  {
    if(v.is<picojson::value::array>())
    {
      vec = vec3_from_js_array(v.get<picojson::value::array>());
      return true;
    }
    else if(v.is<picojson::value::object>())
    {
      vec = vec3_from_js_object(v.get<picojson::value::object>());
      return true;
    }
    else
    {
      if(err) (*err) = "Invalid JSON; expected Vec3 (object or array)";
      return false;
    }
  }
}
