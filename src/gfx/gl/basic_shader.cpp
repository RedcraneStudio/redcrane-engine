/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "basic_shader.h"
namespace strat
{
  namespace gfx
  {
    namespace gl
    {
      // We are directly dependent on a filename here, hmm. Anyone smell that?
      Basic_Shader::Basic_Shader() noexcept
        : prog_(Program::from_files("shader/basic/v", "shader/basic/f"))
      {
        proj_loc_ = prog_.get_uniform_location("proj");
        view_loc_ = prog_.get_uniform_location("view");
        model_loc_ = prog_.get_uniform_location("model");
        diffuse_loc_ = prog_.get_uniform_location("dif");
        sampler_loc_ = prog_.get_uniform_location("tex");
      }

      void Basic_Shader::set_projection(glm::mat4 const& proj) noexcept
      {
        glUniformMatrix4fv(proj_loc_, 1, GL_FALSE, &proj[0][0]);
      }
      void Basic_Shader::set_view(glm::mat4 const& view) noexcept
      {
        glUniformMatrix4fv(view_loc_, 1, GL_FALSE, &view[0][0]);
      }
      void Basic_Shader::set_model(glm::mat4 const& model) noexcept
      {
        glUniformMatrix4fv(model_loc_, 1, GL_FALSE, &model[0][0]);
      }

      void Basic_Shader::set_material(Material const& mat) noexcept
      {
        // Set diffuse color.
        glUniform3f(diffuse_loc_, mat.diffuse_color.r / (float) 0xff,
                    mat.diffuse_color.g / (float) 0xff,
                    mat.diffuse_color.b / (float) 0xff);

        // Bind texture and notify the shader program of that location.
        mat.texture->bind(0);
        glUniform1i(sampler_loc_, 0);
      }
      void Basic_Shader::use() noexcept
      {
        prog_.use();
      }
      void Basic_Shader::render(Prepared_Mesh const& mesh) noexcept
      {
        // Basic_Shader and the (prepared_)mesh are officially coupled, they
        // depend on each other's implementation.
        mesh.render();
      }
    }
  }
}
