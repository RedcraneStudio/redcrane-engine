/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */

#include "minigltf.h"
#include <cstdio>
#include "../common/log.h"

#define TINYGLTF_LOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "../../gltf/tiny_gltf_loader.h"

namespace redc
{
  boost::optional<bool> load_gltf_file(std::string name)
  {
    std::string err;
    tinygltf::Scene scene;

    tinygltf::TinyGLTFLoader loader;

    if(!loader.LoadASCIIFromFile(&scene, &err, name))
    {
      log_e("Error in '%': %", name, err);
      return boost::none;
    }

    return boost::none;
  }
}
