/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include "../common/color.h"
#include "../common/mesh.h" // TODO: Fix this weird tree structure?
#include "../texture.h"
#include "material.h"
#include "camera.h"
namespace strat
{
  namespace gfx
  {
    struct IDriver
    {
      virtual ~IDriver() noexcept {}

      virtual void prepare_mesh(Mesh& mesh) noexcept = 0;
      virtual void remove_mesh(Mesh& mesh) noexcept = 0;
      virtual void render_mesh(Mesh const& mesh) noexcept = 0;

      virtual void prepare_texture(Texture& tex) noexcept = 0;
      virtual void remove_texture(Texture& tex) noexcept = 0;
      virtual void bind_texture(Texture const&, unsigned int) noexcept=0;

      virtual void prepare_material(Material& mat) noexcept = 0;
      virtual void remove_material(Material& mat) noexcept = 0;
      virtual void bind_material(Material const& mat) noexcept = 0;

      virtual void prepare_camera(Camera& cam) noexcept = 0;
      virtual void remove_camera(Camera& cam) noexcept = 0;
      virtual void use_camera(Camera const& cam) noexcept = 0;

      virtual void set_model(glm::mat4 const&) noexcept = 0;

      virtual void clear_color_value(Color const& color) noexcept = 0;
      virtual void clear_depth_value(float val) noexcept = 0;

      virtual void clear() noexcept = 0;
      virtual void clear_color() noexcept = 0;
      virtual void clear_depth() noexcept = 0;
    };
  }
}
