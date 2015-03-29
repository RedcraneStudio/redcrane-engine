/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include "gfx/mesh.h"
#include "gfx/texture.h"
#include "gfx/pipeline.h"
#include <glm/glm.hpp>
namespace survive
{
  struct SceneNode
  {
    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<Texture> texture;

    gfx::Pipeline_Mesh* prepared;

    glm::mat4 model;

    std::vector<std::unique_ptr<SceneNode> > children_;
  };

  SceneNode load_scene(std::string fn) noexcept;
}
