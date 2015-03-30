/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "scene.h"
#include "common/json.h"
#include "common/log.h"
#include <glm/gtc/matrix_transform.hpp>
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
  SceneNode load_scene(T const& doc) noexcept
  {
    auto node = SceneNode{};

    if_has_member(doc, "mesh", [&](auto const& val)
    {
      auto mesh = Mesh::from_file(val.GetString());
      node.mesh.set_owned(std::move(mesh));
    });
#if 0
    if_has_member(doc, "texture", [&](auto const& val)
    {
      auto texture = Texture::from_png_file(val.GetString());
      node.texture = std::make_unique<Texture>(std::move(texture));
    });
#endif
    if_has_member(doc, "translation", [&](auto const& val)
    {
      node.model = load_translation(val);
    });

    return node;
  }

  SceneNode load_scene(std::string fn) noexcept
  {
    auto doc = load_json(fn);
    return load_scene(doc);
  }

  void prepare_scene(gfx::Pipeline& pipeline, SceneNode& scene) noexcept
  {
    // For each node in the scene, prepare it's mesh.
    // But only prepare the mesh if it's owned by the node itself.
    // Ez Pz

    // Do ourselves.
    if(scene.mesh.is_owned())
    {
      scene.mesh.set_pointer(pipeline.prepare_mesh(scene.mesh.unwrap()));
    }

    // Then all of our children.
    for(auto& child : scene.children_)
    {
      prepare_scene(pipeline, *child);
    }
  }

  void remove_scene(gfx::Pipeline& pipeline, SceneNode& scene) noexcept
  {
    if(scene.mesh.is_pointer())
    {
      scene.mesh.set_owned(pipeline.remove_mesh(*scene.mesh));
    }
    for(auto& child : scene.children_)
    {
      remove_scene(pipeline, *child);
    }
  }
}
