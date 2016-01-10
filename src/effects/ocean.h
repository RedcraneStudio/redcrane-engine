/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <chrono>
#include "../use/effect.h"
#include "../water/grid.h"
namespace redc { namespace effects
{
  struct Ocean_Effect : public gfx::Effect
  {
    void init(gfx::IDriver& driver, po::variables_map const& vm) noexcept;
    void render(gfx::IDriver& driver, gfx::Camera const& cam) noexcept;
  private:
    std::size_t elements_;

    std::unique_ptr<Mesh> grid_mesh_;
    std::unique_ptr<gfx::Shader> shader_;

    float max_disp_ = 0.0f;

    int projector_loc_ = 0;
    int time_loc_ = 0;

    int cam_pos_loc_ = 0;
    int light_dir_loc_ = 0;

    water::Plane water_base_;

    std::chrono::high_resolution_clock::time_point start_;
  };
} }

