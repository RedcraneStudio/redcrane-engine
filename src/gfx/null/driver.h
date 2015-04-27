/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../idriver.h"
namespace game
{
  namespace gfx
  {
    namespace null
    {
      struct Driver;
    }
  }

  struct gfx::null::Driver : public gfx::IDriver
  {
    void set_shader(Shader) noexcept override {}

    void prepare_mesh(Mesh&) noexcept override {}
    void remove_mesh(Mesh&) noexcept override {}
    void render_mesh(Mesh const&) noexcept override {}

    void prepare_texture(Texture&) noexcept override {}
    void remove_texture(Texture&) noexcept override {}
    void bind_texture(Texture const&, unsigned int) noexcept override {}

    void set_diffuse(Color const&) noexcept override {}

    void set_projection(glm::mat4 const&) noexcept override {}
    void set_view(glm::mat4 const&) noexcept override {}
    void set_model(glm::mat4 const&) noexcept override {}

    void clear_color_value(Color const&) noexcept override {}
    void clear_depth_value(float) noexcept override {}

    void clear() noexcept override {}
    void clear_color() noexcept override {}
    void clear_depth() noexcept override {}
  };
}
