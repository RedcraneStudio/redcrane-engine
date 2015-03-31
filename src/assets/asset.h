/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include <vector>
#include <memory>

#include "../gfx/prepared_mesh.h"
#include "../gfx/prepared_texture.h"

#include "rapidjson/document.h"

#define DECLARE_EXTENSION(ext_str) \
constexpr static char const* const ext = #ext_str;

namespace survive { namespace assets
{
  struct Asset
  {
    virtual ~Asset() noexcept {}

    std::string name;
    std::string ext;
  };

  struct Mesh_Asset : public Asset
  {
    DECLARE_EXTENSION(obj);

    std::unique_ptr<gfx::Prepared_Mesh> mesh;
  };
  struct Texture_Asset : public Asset
  {
    DECLARE_EXTENSION(png);

    std::unique_ptr<gfx::Prepared_Texture> texture;
  };

  struct Json_Asset : public Asset
  {
    DECLARE_EXTENSION(json);

    rapidjson::Document json;
  };

  struct Font_Asset : public Asset
  {
    DECLARE_EXTENSION(ttf);

    std::string abs_path;
  };

  using Vector = std::vector<std::shared_ptr<Asset> >;
} }
#undef DECLARE_EXTENSION
