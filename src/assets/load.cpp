/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "load.h"

#include "../common/log.h"
#include "../common/json.h"

#include <boost/filesystem.hpp>

namespace redc { namespace assets
{
  namespace fs = boost::filesystem;

  template <class T, class A>
  void do_cross_reference(T& json,
                          std::vector<std::shared_ptr<Asset> >& vec,
                          A& allocator) noexcept
  {
    if(json.IsObject())
    {
      for(auto iter = json.MemberBegin(); iter != json.MemberEnd(); ++iter)
      {
        do_cross_reference(iter->value, vec, allocator);
      }
    }
    else if(json.IsArray())
    {
      for(auto iter = json.Begin(); iter != json.End(); ++iter)
      {
        do_cross_reference(*iter, vec, allocator);
      }
    }
    else if(json.IsString())
    {
      // Match the string?
      auto str = std::string{json.GetString()};

      auto asset_name = std::string{};

      size_t loc;
      auto key_name = std::string{};

      if(str[0] == '@')
      {
        // Go to the colon.
        for(loc = 1; loc < str.size() && str[loc] != ':'; ++loc)
        {
          asset_name.push_back(str[loc]);
        }
        // Go to the end of the string.
        for(++loc; loc < str.size(); ++loc)
        {
          key_name.push_back(str[loc]);
        }

        // Set the new string, so we first need to find the referring asset.
        // Bail out on error
        for(auto asset : vec)
        {
          if(asset->name != asset_name) continue;

          // The asset is the one we are looking for.

          // Make sure it's json
          auto json_asset = std::dynamic_pointer_cast<Json_Asset>(asset);
          if(json_asset)
          {
            if(json_asset->json.HasMember(key_name.data()))
            {
              auto const& match = json_asset->json[key_name.data()];
              if(match.IsString())
              {
                auto match_str = std::string{match.GetString()};
                log_d("Found: '" + str + "' = '" + match_str + "'");
              }
              else
              {
                log_d("Found: '" + str + "'");
              }
              json.CopyFrom(match, allocator);
            }
            else break;
          }
          // We can break because no other asset could have this name.
          else break;
        }
      }

    }
  }
  assets::Vector load(std::vector<std::string> const& assets,
                      gfx::IDriver& fact) noexcept
  {
    auto ret = std::vector<std::shared_ptr<Asset> >{};

    for(auto asset : assets)
    {
      auto path = fs::path{asset};

      // Get the info about it.
      auto file = path.stem().native();
      auto ext = path.extension().native();
      ext.erase(ext.begin()); // Remove the dot in the extension.

      auto fn = path.native();

      auto asset_ptr = std::shared_ptr<Asset>{nullptr};
      if(ext == "json")
      {
        auto json_asset = std::make_shared<Json_Asset>();

        // Load the json
        try
        {
          json_asset->json = load_json(fn);
        }
        catch(Bad_Asset& e)
        {
          // Log an error.
          log_e("Failed to load json ('%')\n"
                "Error at position: %\n%", fn, e.error_offset, e.diagnostic);
          continue;
        }

        asset_ptr = json_asset;
      }
      else if(ext == "obj")
      {
        auto mesh_asset = std::make_shared<Mesh_Asset>();
        mesh_asset->mesh = fact.prepare_mesh(Mesh::from_file(fn));
        asset_ptr = mesh_asset;
      }
      else if(ext == "png")
      {
        auto tex_asset = std::make_shared<Texture_Asset>();

        // Load the image.
        tex_asset->texture = fact.prepare_texture(Texture::from_png_file(fn));

        // Log for failure.
        if(!tex_asset->texture->texture().data)
        {
          log_e("Failed to load texture ('" + fn + "')");
          continue;
        }

        asset_ptr = tex_asset;
      }
      else if(ext == "ttf")
      {
        auto font_asset = std::make_shared<Font_Asset>();

        font_asset->abs_path = fs::absolute(path).native();

        asset_ptr = font_asset;
      }
      else
      {
        log_d("Invalid extension: '" + ext + "'; Not loading.");
        continue;
      }

      // Uses every part of the path except the extension.
      asset_ptr->name = (path.parent_path() / fs::path{file}).native();
      asset_ptr->ext = ext;

      log_i("Successfully loaded '" + fn + "'");

      ret.push_back(asset_ptr);
    }

    // Do any "cross-referencing." This is where we check for any string values
    // in the json with a format of @asset:key. The asset is retrieved and the
    // value of that key is put in place of that code in the original json file
    // if that makes sense.
    // TODO: write this down, this is some solid "algorithmic thinking" if I do
    // say so myself.
    for(auto asset : ret)
    {
      auto json = std::dynamic_pointer_cast<Json_Asset>(asset);
      if(json)
      {
        if(!json->json.IsObject()) continue;
        if(!json->json.HasMember("cross_reference")) continue;
        if(json->json["cross_reference"].GetBool() == false) continue;

        log_d("Cross-referencing for " + asset->name);
        do_cross_reference(json->json, ret, json->json.GetAllocator());
      }
    }

    return ret;
  }
} }

