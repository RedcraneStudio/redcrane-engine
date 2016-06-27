/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */

// Include the implementation first so that it actually works. We have to do
// this because our header file includes the header as well, we might as well
// put the implementation here.
#define TINYGLTF_LOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "../../gltf/tiny_gltf_loader.h"

#include "minigltf.h"

#include <cstdio>
#include "../common/log.h"

namespace redc
{
  bool load_gltf_file(Scene& scene, std::string const& name)
  {
    std::string err;

    tinygltf::TinyGLTFLoader loader;

    if(!loader.LoadASCIIFromFile(&scene, &err, name,
                                 tinygltf::NO_REQUIRE))
    {
      // There was a failure loading
      log_e("Error in '%': %", name, err);
      return false;
    }
    else if(err.size() > 0)
    {
      // There was information, but no failure.
      log_i("Information loading '%': %", name, err);
    }

    return true;
  }
  boost::optional<Scene> load_gltf_file(std::string const& name)
  {
    boost::optional<Scene> scene = Scene{};
    if(load_gltf_file(*scene.get_ptr(), name))
    {
      return scene;
    }
    else
    {
      return boost::none;
    }
  }
}
