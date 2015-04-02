/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "scene_node.h"
#include "common/json.h"
#include "common/log.h"
#include <glm/gtc/matrix_transform.hpp>
#include "gfx/imaterial.h"
namespace survive
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
  Scene_Node load_scene(T const& doc, gfx::IDriver& driver) noexcept
  {
    auto node = Scene_Node{};

    if_has_member(doc, "mesh", [&](auto const& val)
    {
      auto mesh = Mesh::from_file(val.GetString());
      node.obj.mesh = driver.prepare_mesh(std::move(mesh));
    });
    if_has_member(doc, "material", [&](auto const& val)
    {
      node.obj.material = gfx::load_material(val.GetString());
    });
    if_has_member(doc, "translation", [&](auto const& val)
    {
      node.model = load_translation(val);
    });

    return node;
  }

  Scene_Node load_scene(std::string fn, gfx::IDriver& d) noexcept
  {
    auto doc = load_json(fn);
    return load_scene(doc, d);
  }

  void prepare_scene(Scene_Node& scene, gfx::IDriver& driver) noexcept
  {
    // Do ourselves, then our children.
    if(scene.obj.mesh)
    {
      // Unprepare the mesh from what is likely a null driver and prepare it
      // for whatever the driver we got supports.
      scene.obj.mesh = driver.prepare_mesh(scene.obj.mesh->unwrap());
    }

    for(auto const& child : scene.children)
    {
      if(child)
      {
        prepare_scene(*child, driver);
      }
    }
  }

  void render_scene(Scene_Node& scene) noexcept
  {
    render_object(scene.obj);

    for(auto const& child : scene.children)
    {
      render_scene(*child);
    }
  }
}
