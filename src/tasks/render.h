/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <string>
#include "../common/task.h"
#include "../common/vec.h"
#include "SDL.h"
#include "../sail/game_struct.h"
#include "../gfx/gl/driver.h"

#include "../common/aabb.h"

#include "../effects/envmap.h"
#include "../effects/ocean.h"
#include "../effects/boat.h"

#include "render/sdl_helper.h"
namespace redc
{
  struct Render_Task : public Task
  {
    Render_Task(sail::Game const& game, po::variables_map const& vm,
                std::string title, Vec<int> res, bool fullscreen,
                bool vsync) noexcept;
    void step(float dt) noexcept override;

    bool should_close() noexcept;
  private:
    sail::Game const* game_;
    SDL_Init_Lock sdl;

    bool should_close_ = false;

    std::unique_ptr<gfx::gl::Driver> driver_;
    std::unique_ptr<gfx::Mesh_Cache> mesh_cache_;

    effects::Envmap_Effect envmap_;
    effects::Ocean_Effect ocean_;
    effects::Boat_Effect boat_;

    AABB boat_aabb_;

    gfx::Camera cam_;
  };
}
