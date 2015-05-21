/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include "../common/color.h"
#include "../common/texture.h"
#include "../common/mesh.h"
namespace game
{
  namespace gfx
  {
    enum class Shader
    {
      Standard, Hud
    };

    struct IDriver
    {
      virtual ~IDriver() noexcept {}

      virtual void set_shader(Shader shade) noexcept = 0;

      virtual std::unique_ptr<Mesh> make_mesh_repr() noexcept = 0;
      virtual void render_mesh(Mesh& mesh) noexcept = 0;
      virtual void render_mesh(Mesh& mesh, std::size_t start,
                               std::size_t count) noexcept = 0;

      virtual std::unique_ptr<Texture> make_texture_repr() noexcept = 0;
      virtual void bind_texture(Texture& tex, unsigned int loc) noexcept = 0;

      virtual void set_diffuse(Color const&) noexcept = 0;

      virtual void set_projection(glm::mat4 const&) noexcept = 0;
      virtual void set_view(glm::mat4 const&) noexcept = 0;
      virtual void set_model(glm::mat4 const&) noexcept = 0;

      virtual void clear_color_value(Color const& color) noexcept = 0;
      virtual void clear_depth_value(float val) noexcept = 0;

      virtual void clear() noexcept = 0;
      virtual void clear_color() noexcept = 0;
      virtual void clear_depth() noexcept = 0;

      virtual void depth_test(bool enable) noexcept = 0;
      virtual void blending(bool enable) noexcept = 0;

      virtual Vec<int> window_extents() const noexcept = 0;
    };
  }
}
