/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <unordered_map>
#include "../idriver.h"
namespace game
{
  namespace gfx
  {
    namespace gl
    {
      struct Driver;
    }
  }

  struct gfx::gl::Driver : public gfx::IDriver
  {
    Driver(Vec<int> size) noexcept;
    ~Driver() noexcept;

    std::unique_ptr<Shader> make_shader_repr() noexcept override;
    void set_shader(Shader&) noexcept override;
    Shader* active_shader() const noexcept override;

    std::unique_ptr<Mesh> make_mesh_repr() noexcept override;
    void render_mesh(Mesh& mesh) noexcept override;
    void render_mesh(Mesh& mesh, std::size_t start,
                     std::size_t count) noexcept override;

    std::unique_ptr<Texture> make_texture_repr() noexcept override;
    void bind_texture(Texture& tex, unsigned int loc) noexcept override;

    void clear_color_value(Color const&) noexcept override;
    void clear_depth_value(float) noexcept override;

    void clear() noexcept override;
    void clear_color() noexcept override;
    void clear_depth() noexcept override;

    void depth_test(bool enable) noexcept override;
    void blending(bool enable) noexcept override;
    void face_culling(bool enable) noexcept override;

    Vec<int> window_extents() const noexcept override { return extents_; }

    void check_error() noexcept override;
  private:
    Vec<int> extents_;

    Shader* cur_shader_;
  };
}
