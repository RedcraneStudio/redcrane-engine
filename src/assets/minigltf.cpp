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
  bool load_gltf_file(tinygltf::Scene& scene, std::string const& name)
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
  boost::optional<tinygltf::Scene> load_gltf_file(std::string const& name)
  {
    boost::optional<tinygltf::Scene> scene = tinygltf::Scene{};
    if(load_gltf_file(*scene.get_ptr(), name))
    {
      return scene;
    }
    else
    {
      return boost::none;
    }
  }

  bool resolve_gltf_accessor_data(tinygltf::Scene const& scene,
                                  std::string const& accessor,
                                  std::vector<uint8_t>& data,
                                  tinygltf::Accessor& access_out)
  {
    // Find the accessor
    auto access_find = scene.accessors.find(accessor);

    if(access_find == scene.accessors.end())
    {
      // Failed to find accessor
      return false;
    }

    // Accessor found
    tinygltf::Accessor const& access = access_find->second;

    // Now find the bufferView
    auto buf_view_find =
      scene.bufferViews.find(access.bufferView);

    if(buf_view_find == scene.bufferViews.end())
    {
      // Failed to find buffer view
      return false;
    }

    // Buffer view found
    tinygltf::BufferView const& buf_view = buf_view_find->second;

    // Now find the buffer
    auto buf_find = scene.buffers.find(buf_view.buffer);

    if(buf_find == scene.buffers.end())
    {
      // Failed to find buffer
      return false;
    }

    // Buffer found
    tinygltf::Buffer buf = buf_find->second;

    // Find the offset and length (only consider the buffer view, because we are
    // returning the accessor so the client code can do the rest.
    std::size_t offset = buf_view.byteOffset;
    std::size_t length = buf_view.byteLength;

    // Now copy the data
    data.resize(length);
    std::copy(&buf.data[offset], &buf.data[offset] + length, data.begin());

    // And the accessor
    access_out = access;

    // Success!
    return true;
  }
}
