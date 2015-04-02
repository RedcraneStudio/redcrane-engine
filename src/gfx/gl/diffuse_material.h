/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include "../imaterial.h"
#include "program.h"
#include "../../color.h"
#include "glad/glad.h"
namespace survive
{
  namespace gfx
  {
    namespace gl
    {
      struct Diffuse_Material;
    }
  }

  struct gfx::gl::Diffuse_Material : public IMaterial
  {
    Diffuse_Material() noexcept;
    void use() const noexcept override;

    void diffuse_color(Color const& c) noexcept;
    Color const& diffuse_color() const noexcept;

    void set_projection(glm::mat4 const& proj) noexcept override;
    void set_view(glm::mat4 const& view) noexcept override;
  private:
    mutable std::shared_ptr<Program> prog_;

    GLint projection_matrix_loc_;
    GLint view_matrix_loc_;

    mutable bool diffuse_color_changed_;
    GLint diffuse_color_loc_;
    Color diffuse_color_;
  };
}
