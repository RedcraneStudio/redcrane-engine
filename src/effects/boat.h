/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../use/effect.h"
#include "../use/mesh_cache.h"
#include "../boatlib/boat.h"
#include "../collisionlib/motion.h"
namespace redc { namespace effects
{
  struct Boat_Effect : public gfx::Effect
  {
    void init(gfx::IDriver& driver, po::variables_map const&) noexcept;
    void render(gfx::IDriver& driver, gfx::Camera const& cam) noexcept;

    collis::Motion motion;
  private:
    std::unique_ptr<gfx::Shader> shader_;
    int light_pos_loc_;

    Boat_Descs boat_descs_;
    Boat_Config boat_config_;
    Boat_Render_Config boat_render_config_;
  };
} }
