/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <unordered_map>
#include "../idriver.h"
#include "prep_mesh.h"
#include "prep_tex.h"
#include "basic_shader.h"
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
    Driver() noexcept;
    ~Driver() noexcept;

    void prepare_mesh(Mesh& mesh) noexcept override;
    void remove_mesh(Mesh& mesh) noexcept override;
    void render_mesh(Mesh const& mesh) noexcept override;

    void prepare_texture(Texture& tex) noexcept override;
    void remove_texture(Texture& tex) noexcept override;
    void bind_texture(Texture const& tex, unsigned int l) noexcept override;

    void prepare_material(Material& mat) noexcept override;
    void remove_material(Material& mat) noexcept override;
    void bind_material(Material const& mat) noexcept override;

    void prepare_camera(Camera& cam) noexcept override;
    void remove_camera(Camera& cam) noexcept override;
    void use_camera(Camera const& cam) noexcept override;

    void set_projection(glm::mat4 const& p) noexcept override;
    void set_view(glm::mat4 const& v) noexcept override;
    void set_model(glm::mat4 const&) noexcept override;

    void clear_color_value(Color const&) noexcept override;
    void clear_depth_value(float) noexcept override;

    void clear() noexcept override;
    void clear_color() noexcept override;
    void clear_depth() noexcept override;
  private:
    std::unordered_map<Mesh const*, Prep_Mesh> meshs_;
    std::unordered_map<Texture const*, Prep_Tex> textures_;

    Basic_Shader shader_;
  };
}
