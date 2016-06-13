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
#include "../../gltf/tiny_gltf_loader.h"
namespace redc
{
  namespace fs = boost::filesystem;

  boost::optional<tinygltf::Scene> load_gltf_file(std::string name);
}
