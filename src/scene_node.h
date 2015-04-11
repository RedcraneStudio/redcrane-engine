/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <vector>
#include <string>
#include "gfx/object.h"
#include "gfx/idriver.h"
namespace strat
{
  struct Scene_Node
  {
    // It is on the stack, but remember an object has an optional mesh and
    // material. Meaning an object may have a material and no mesh. Then, each
    // child could lack a mesh making this one the active one for all of them.
    gfx::Object obj;

    std::vector<std::unique_ptr<Scene_Node> > children;
  };

  Scene_Node load_scene(std::string fn) noexcept;

  void prepare_scene(gfx::IDriver&, Scene_Node&) noexcept;
  void remove_scene(gfx::IDriver&, Scene_Node&) noexcept;

  void render_scene(gfx::IDriver&, Scene_Node const& scene) noexcept;
}
