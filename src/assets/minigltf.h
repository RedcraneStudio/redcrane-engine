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

  bool load_gltf_file(tinygltf::Scene& scene, std::string const& filename);
  boost::optional<tinygltf::Scene> load_gltf_file(std::string const& name);

  bool resolve_gltf_accessor_data(tinygltf::Scene const& scene,
                                  std::string const& accessor,
                                  std::vector<uint8_t>& data,
                                  tinygltf::Accessor& access_out);
}
