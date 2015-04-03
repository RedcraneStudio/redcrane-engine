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
#include "gfx/scene.h"
#include "maybe_owned.hpp"
namespace survive
{
  struct Scene_Node
  {
    // It is on the stack, but remember an object has an optional mesh and
    // material. Meaning an object may have a material and no mesh. Then, each
    // child could lack a mesh making this one the active one for all of them.
    gfx::Object obj;

    std::vector<std::unique_ptr<Scene_Node> > children;

    void render() const noexcept;
  private:
    void render_with_model_(glm::mat4 const& parent_model) const noexcept;
  };

  /*!
   * \brief Prepares the scene using a given driver.
   *
   * A null driver can be used to "unprepare" the whole scene.
   */
  void prepare_scene(Scene_Node&, gfx::IDriver&) noexcept;

  Scene_Node load_scene(std::string fn, gfx::IDriver&, gfx::Scene&) noexcept;
}
