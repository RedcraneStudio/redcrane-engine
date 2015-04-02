/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "diffuse_material.h"
#include "program_cache.h"
namespace survive
{
  namespace gfx
  {
    namespace gl
    {
      Diffuse_Material::Diffuse_Material() noexcept
        : prog_(load_program("shader/diffuse/decl.json"))
      {
        diffuse_color_loc_ = prog_->get_uniform_location("diffuse");
        diffuse_color_changed_ = true;

        projection_matrix_loc_ = prog_->get_uniform_location("proj");
        view_matrix_loc_ = prog_->get_uniform_location("view");
        model_matrix_loc_ = prog_->get_uniform_location("model");
      }
      void Diffuse_Material::use(glm::mat4 const& m) const noexcept
      {
        if(diffuse_color_changed_)
        {
          glUniform3f(diffuse_color_loc_, diffuse_color_.r / (float) 0xff,
                      diffuse_color_.g / (float) 0xff,
                      diffuse_color_.b / (float) 0xff);
          diffuse_color_changed_ = false;
        }

        glUniformMatrix4fv(model_matrix_loc_, 1, GL_FALSE, &m[0][0]);

        prog_->use();
      }

      void Diffuse_Material::diffuse_color(Color const& c) noexcept
      {
        if(c != diffuse_color_)
        {
          diffuse_color_changed_ = true;
          diffuse_color_ = c;
        }
      }
      Color const& Diffuse_Material::diffuse_color() const noexcept
      {
        return diffuse_color_;
      }
      void Diffuse_Material::set_projection(glm::mat4 const& proj) noexcept
      {
        glUniformMatrix4fv(projection_matrix_loc_, 1, GL_FALSE, &proj[0][0]);
      }
      void Diffuse_Material::set_view(glm::mat4 const& view) noexcept
      {
        glUniformMatrix4fv(view_matrix_loc_, 1, GL_FALSE, &view[0][0]);
      }
    }
  }
}
