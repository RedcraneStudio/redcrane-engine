/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include "../common/vec.h"
#include "texture.h"
#include "mesh.h"
#include "shader.h"
namespace game
{
  // Forward declare the color structure.
  struct Color;

  namespace gfx
  {
    enum class Framebuffer
    {
      Depth
    };

    struct IDriver
    {
      virtual ~IDriver() noexcept {}

      virtual std::unique_ptr<Shader> make_shader_repr() noexcept = 0;
      virtual void use_shader(Shader&) noexcept = 0;
      virtual Shader* active_shader() const noexcept = 0;

      virtual std::unique_ptr<Mesh> make_mesh_repr() noexcept = 0;
      virtual void bind_mesh(Mesh& mesh) noexcept = 0;

      virtual std::unique_ptr<Texture> make_texture_repr() noexcept = 0;
      virtual void bind_texture(Texture& tex, unsigned int loc) noexcept = 0;

      virtual void clear_color_value(Color const& color) noexcept = 0;
      virtual void clear_depth_value(float val) noexcept = 0;

      virtual void clear() noexcept = 0;
      virtual void clear_color() noexcept = 0;
      virtual void clear_depth() noexcept = 0;

      virtual void depth_test(bool enable) noexcept = 0;
      virtual void blending(bool enable) noexcept = 0;
      virtual void face_culling(bool enable) noexcept = 0;

      // For now we are only going to abstract a single point in the
      // framebuffer. In addition, it'll get a bit complicated should we want
      // to store the result in GL_PIXEL_PACK_BUFFER or maybe a shared buffer
      // in client memory and just return a pointer to that, for all who
      // request it. Who knows.

      virtual float read_pixel(Framebuffer fr, Vec<int> vec) noexcept = 0;

      virtual Vec<int> window_extents() const noexcept = 0;

      virtual void check_error() noexcept = 0;
    };
  }
}
