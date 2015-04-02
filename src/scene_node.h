/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <vector>
#include <string>
#include "gfx/object.h"
#include "gfx/idriver.h"
#include "maybe_owned.hpp"
namespace survive
{
  struct Scene_Node
  {
    // It is on the stack, but remember an object has an optional mesh and
    // material. Meaning an object may have a material and no mesh. Then, each
    // child could lack a mesh making this one the active one for all of them.
    gfx::Object obj;
    glm::mat4 model;

    std::vector<std::unique_ptr<Scene_Node> > children;
  };

  /*!
   * \brief Prepares the scene using a given driver.
   *
   * A null driver can be used to "unprepare" the whole scene.
   */
  void prepare_scene(Scene_Node&, gfx::IDriver&) noexcept;

  void render_scene(Scene_Node& scene) noexcept;

  Scene_Node load_scene(std::string fn, gfx::IDriver&) noexcept;
}
