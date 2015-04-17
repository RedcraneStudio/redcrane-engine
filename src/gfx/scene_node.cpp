/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "scene_node.h"
#include "../common/json.h"
#include "../common/log.h"
#include <glm/gtc/matrix_transform.hpp>
namespace game
{
  namespace gfx
  {
    template <class T>
    glm::mat4 load_translation(T const& doc) noexcept
    {
      auto x = float(doc.HasMember("x") ? doc["x"].GetDouble() : 0.0);
      auto y = float(doc.HasMember("y") ? doc["y"].GetDouble() : 0.0);
      auto z = float(doc.HasMember("z") ? doc["z"].GetDouble() : 0.0);

      return glm::translate(glm::mat4(1.0), glm::vec3(x, y, z));
    }

    template <class T>
    Scene_Node load_scene(T const& doc) noexcept
    {
      auto node = Scene_Node{};

      // Load the mesh with a filename, then prepare it with the driver.
      if_has_member(doc, "mesh", [&](auto const& val)
      {
        auto mesh = Mesh::from_file(val.GetString());
        node.obj.mesh = Maybe_Owned<Mesh>(std::move(mesh));
      });
      // Load some basic properties of the material.
      if_has_member(doc, "material", [&](auto const& val)
      {
        auto mat = gfx::load_material(val.GetString());
        node.obj.material = Maybe_Owned<gfx::Material>(std::move(mat));
      });
      // Load an initial translation if it exists.
      if_has_member(doc, "translation", [&](auto const& val)
      {
        node.obj.model_matrix = load_translation(val);
      });

      return node;
    }

    Scene_Node load_scene(std::string fn) noexcept
    {
      auto doc = load_json(fn);
      return load_scene(doc);
    }

    void prepare_scene(gfx::IDriver& d, Scene_Node& sn) noexcept
    {
      prepare_object(d, sn.obj);
      for(auto& child : sn.children)
      {
        prepare_scene(d, *child);
      }
    }
    void remove_scene(gfx::IDriver& d, Scene_Node& sn) noexcept
    {
      remove_object(d, sn.obj);
      for(auto& child : sn.children)
      {
        remove_scene(d, *child);
      }
    }

    void render_scene_with_model(gfx::IDriver& d, Scene_Node const& scene,
                                 glm::mat4 const& par_mod) noexcept
    {
      // "cause you don't understand bb, bumbumbumbumbum" ~ Selah Sue
      auto this_world = par_mod * scene.obj.model_matrix;
      // TODO Cache this ^

      render_object(d, scene.obj, this_world);

      for(auto const& child : scene.children)
      {
        render_scene_with_model(d, scene, this_world);
      }
    }
    void render_scene(gfx::IDriver& d, Scene_Node const& scene) noexcept
    {
      render_object(d, scene.obj);

      for(auto const& child : scene.children)
      {
        render_scene_with_model(d, *child, scene.obj.model_matrix);
      }
    }
  }
}
