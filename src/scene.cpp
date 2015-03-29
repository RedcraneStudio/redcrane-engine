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
      node.mesh = std::make_unique<Mesh>(std::move(mesh));
    });
    if_has_member(doc, "texture", [&](auto const& val)
    {
      auto texture = Texture::from_png_file(val.GetString());
      node.texture = std::make_unique<Texture>(std::move(texture));
    });
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
}
