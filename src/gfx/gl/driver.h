/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <unordered_map>
#include "../idriver.h"
#include "ishader.h"
#include "hud_shader.h"
#include "standard_shader.h"
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

    void set_shader(Shader shade) noexcept override;

    std::unique_ptr<Mesh> make_mesh_repr() noexcept override;
    void render_mesh(Mesh& mesh) noexcept override;

    std::unique_ptr<Texture> make_texture_repr() noexcept override;
    void bind_texture(Texture& tex, unsigned int loc) noexcept override;

    void set_diffuse(Color const&) noexcept override;

    void set_projection(glm::mat4 const& p) noexcept override;
    void set_view(glm::mat4 const& v) noexcept override;
    void set_model(glm::mat4 const&) noexcept override;

    void clear_color_value(Color const&) noexcept override;
    void clear_depth_value(float) noexcept override;

    void clear() noexcept override;
    void clear_color() noexcept override;
    void clear_depth() noexcept override;
  private:
    IShader* current_shader_;

    Standard_Shader standard_shader_;
    Hud_Shader hud_shader_;
  };
}
