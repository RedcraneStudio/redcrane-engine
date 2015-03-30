/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include "mesh.h"
#include "texture.h"
#include "maybe_owned.hpp"
#include "gfx/pipeline.h"
#include <glm/glm.hpp>
namespace survive
{
  struct SceneNode
  {
    // Add a maybe-owned pointer to a mesh.
    Maybe_Owned<Mesh> mesh;
    //std::unique_ptr<Texture> texture;

    glm::mat4 model;

    std::vector<std::unique_ptr<SceneNode> > children_;
  };

  void prepare_scene(gfx::Pipeline& pipeline, SceneNode& scene) noexcept;
  void remove_scene(gfx::Pipeline& pipeline, SceneNode& scene) noexcept;
  SceneNode load_scene(std::string fn) noexcept;
}
