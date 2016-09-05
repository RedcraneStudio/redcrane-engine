/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include <vector>
#include <memory>
#include "idriver.h"
#include "scene.h"
#include "common.h"
#include "deferred.h"
namespace redc { namespace gfx
{
  struct Param_Override
  {
    Param_Override(std::string const& name) : name(name) {}

    std::string name;
    Value value;
  };

  struct Rendering_State
  {
    Technique_Ref cur_technique_i = -1;
    Material_Ref cur_material_i = -1;

    std::vector<Param_Override> overrides;

    IDriver* driver;
    std::unique_ptr<Deferred_Shading> deferred;
  };

  void render_asset(Asset const& asset, Camera const& camera,
                    Rendering_State& cur_rendering_state);
} }
