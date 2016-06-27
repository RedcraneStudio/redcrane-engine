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

  //! An asset in CPU memory, contains information verbatim from gltf file.
  using Scene = tinygltf::Scene;

  bool load_gltf_file(Scene& scene, std::string const& name);
  boost::optional<Scene> load_gltf_file(std::string const& name);
}
