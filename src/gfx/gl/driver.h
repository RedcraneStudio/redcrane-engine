/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <unordered_map>
#include "../idriver.h"
namespace redc
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
    void use_shader(Shader&) noexcept override;
    Shader* active_shader() const noexcept override;

    std::unique_ptr<IMesh> make_mesh_repr() noexcept override;
    void bind_mesh(IMesh& mesh) noexcept override;

    std::unique_ptr<Texture> make_texture_repr() noexcept override;
    void bind_texture(Texture& tex, unsigned int loc) noexcept override;

    void set_clear_color(Color const&) noexcept override;
    void set_clear_depth(float) noexcept override;

    void clear() noexcept override;
    void clear_color() noexcept override;
    void clear_depth() noexcept override;

    void depth_test(bool enable) noexcept override;
    void write_depth(bool enable) noexcept override;
    void blending(bool enable) noexcept override;
    void face_culling(bool enable) noexcept override;
    void cull_side(Cull_Side side) noexcept override;

    float read_pixel(Framebuffer, Vec<int> pt) noexcept override;

    void check_error() noexcept override;

  private:

    Shader* cur_shader_;
    IMesh* cur_mesh_;
  };
}
