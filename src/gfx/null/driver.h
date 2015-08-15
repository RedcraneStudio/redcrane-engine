/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../idriver.h"
namespace game { namespace gfx
{
  namespace null
  {
    struct Driver : public IDriver
    {
      Driver(Vec<int> extents) noexcept : IDriver(extents) {}

      std::unique_ptr<Shader> make_shader_repr() noexcept override;
      void use_shader(Shader&) noexcept override;
      Shader* active_shader() const noexcept override;

      std::unique_ptr<Mesh> make_mesh_repr() noexcept override;
      void bind_mesh(Mesh&) noexcept override {}

      std::unique_ptr<Texture> make_texture_repr() noexcept override;
      void bind_texture(Texture&, unsigned int) noexcept override {}

      void clear_color_value(Color const&) noexcept override {}
      void clear_depth_value(float) noexcept override {}

      void clear() noexcept override {}
      void clear_color() noexcept override {}
      void clear_depth() noexcept override {}

      void depth_test(bool) noexcept override {}
      void blending(bool) noexcept override {}
      void face_culling(bool) noexcept override {}

      float read_pixel(Framebuffer, Vec<int>) noexcept override
      { return 0.0f; }

      void check_error() noexcept override {}

    private:

      Shader* cur_shader_;
    };
  }
} }
