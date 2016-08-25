/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../gfx/imesh.h"
#include "../use/effect.h"
namespace redc { namespace effects
{
  struct Envmap_Effect : public gfx::Effect
  {
    void init(gfx::IDriver& driver) noexcept;
    void render(gfx::IDriver& driver, gfx::Camera const& cam) noexcept;
  private:
    std::size_t elements_ = 0;
    std::unique_ptr<gfx::IBuffer> pos_buf_;
    std::unique_ptr<gfx::IMesh> mesh_;
    std::unique_ptr<gfx::ITexture> envmap_;
    std::unique_ptr<gfx::IShader> shader_;
  };
} }
