/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 */
#include "map.h"
#include "cwrap/redcrane.hpp"
#include "assets/minigltf.h"

// For js => vec3 code
#include "gfx/extra/json.h"

// For format(...)
#include "common/translate.h"
namespace redc
{
  bool load_spawns(Spawns_Decl& spawns, rapidjson::Value const& val,
                   std::string* err)
  {
    rapidjson::Value const& choice_val = val["choice"];
    if(!choice_val.IsString())
    {
      if(err) *err = "Spawn location choice must be a string";
      return false;
    }

    if(strcmp(choice_val.GetString(), "random") == 0)
    {
      spawns.choice = Spawns_Decl::Random;
    }
    else
    {
      // Unknown choice value
      if(err)
      {
        *err = fmt("Unknown choice type '%'", choice_val.GetString());
      }
      return false;
    }

    // Parse positions
    rapidjson::Value const& positions_val = val["positions"];
    if(!positions_val.IsArray())
    {
      if(err) *err = "Positions must be an array of positions";
      return false;
    }

    // Get rid of existing positions
    spawns.positions.clear();

    rapidjson::Value::ConstValueIterator iter = positions_val.Begin();
    rapidjson::Value::ConstValueIterator end_iter = positions_val.End();
    for(; iter != end_iter; ++iter)
    {
      // Each one must also be an array with three elements.
      // That is, if one of those conditions doesn't hold, make an error
      if(!iter->IsArray() || iter->Size() != 3)
      {
        if(err) *err = "Position must be an array with three elements";
        return false;
      }

      // Parse each element as a vector
      glm::vec3 pos;
      pos.x = (float) (*iter)[0].GetDouble();
      pos.y = (float) (*iter)[1].GetDouble();
      pos.z = (float) (*iter)[2].GetDouble();

      // This is our position
      spawns.positions.push_back(pos);
    }

    return true;
  }

  bool load_map_json(Map& map, rapidjson::Value const& doc, std::string* err)
  {
    rapidjson::Value const* val;

    // Name
    val = &doc["name"];
    if(val->IsString())
    {
      map.name = std::string{val->GetString(), val->GetStringLength()};
    }
    else
    {
      if(err) *err = "Name must be given as a string";
      return false;
    }

    // Asset
    val = &doc["asset"];
    if(val->IsString())
    {
      std::string gltfname{val->GetString(), val->GetStringLength()};
      // Load the gltf scene file
      if(!load_gltf_file(map.scene, gltfname))
      {
        if(err) *err = "Failed to load glTF asset";
        return false;
      }
    }
    else
    {
      if(err) *err = "glTF asset must be given as a string";
      return false;
    }

    val = &doc["players"];
    if(val->IsUint())
    {
      map.players = static_cast<short>(val->GetUint64());
    }
    else
    {
      if(err) *err = "Player count must be given as a positive integer";
      return false;
    }

    // Will set the error message on failure
    if(!load_spawns(map.spawns, doc["spawn"], err))
    {
      return false;
    }

    val = &doc["collision_mesh"];
    map.collision_vertices_source = (*val)["vertices"].GetString();
    map.collision_indices_source = (*val)["indices"].GetString();

    // Use earth gravity by default.
    map.physics.gravity = glm::vec3(0.0f, -9.81, 0.0f);
    val = &doc["physics"];
    if(val->HasMember("gravity"))
    {
      rapidjson::Value const& gravity_val = (*val)["gravity"];
      if(!gravity_val.IsArray())
      {
        if(err) *err = "Gravity must be a three-element array";
        return false;
      }

      map.physics.gravity = vec3_from_js_array(gravity_val);
    }

    return true;
  }
}
