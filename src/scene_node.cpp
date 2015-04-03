/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "scene_node.h"
#include "common/json.h"
#include "common/log.h"
#include <glm/gtc/matrix_transform.hpp>
#include "gfx/gl/basic_shader.h"
namespace strat
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
  Scene_Node load_scene(T const& doc, gfx::IDriver& driver,
                        gfx::Scene scene_data) noexcept
  {
    auto node = Scene_Node{};

    // This is dependent on OpenGL, so we need to offload this work to the
    // driver!
    node.obj.shader = std::make_shared<gfx::gl::Basic_Shader>();
    node.obj.shader->use();
    // Tell the scene data about our new shader so we get updates when the
    // scene changes.
    scene_data.register_observer(*node.obj.shader);

    // Load the mesh with a filename, then prepare it with the driver.
    if_has_member(doc, "mesh", [&](auto const& val)
    {
      auto mesh = Mesh::from_file(val.GetString());
      node.obj.mesh = driver.prepare_mesh(std::move(mesh));
    });
    // Load some basic properties of the material.
    if_has_member(doc, "material", [&](auto const& val)
    {
      node.obj.material = gfx::load_material(driver, val.GetString());
    });
    // Load an initial translation if it exists.
    if_has_member(doc, "translation", [&](auto const& val)
    {
      node.obj.model_matrix = load_translation(val);
    });

    return node;
  }

  Scene_Node load_scene(std::string fn, gfx::IDriver& d,
                       gfx::Scene& s) noexcept
  {
    auto doc = load_json(fn);
    return load_scene(doc, d, s);
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

  void Scene_Node::render() const noexcept
  {
    render_object(obj);

    for(auto const& child : children)
    {
      child->render_with_model_(*obj.model_matrix);
    }
  }
  void Scene_Node::render_with_model_(glm::mat4 const& par_mod) const noexcept
  {
    // "cause you don't understand bb, bumbumbumbumbum" ~ Selah Sue
    auto this_world = par_mod * *obj.model_matrix;
    // TODO Cache this ^
    render_object(obj, this_world);

    for(auto const& child : children)
    {
      child->render_with_model_(this_world);
    }
  }
}
