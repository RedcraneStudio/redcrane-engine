/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../use/effect.h"
namespace redc { namespace effects
{
  struct Envmap_Effect : public gfx::Effect
  {
    void init(gfx::IDriver& driver, po::variables_map const& vm) noexcept;
    void render(gfx::IDriver& driver, gfx::Camera const& cam) noexcept;
  private:
    std::size_t elements_ = 0;
    std::unique_ptr<Mesh> mesh_;
    std::unique_ptr<Texture> envmap_;
    std::unique_ptr<gfx::Shader> shader_;
    int view_loc_ = 0;
    int proj_loc_ = 0;
  };
} }
