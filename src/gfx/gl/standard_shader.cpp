/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "standard_shader.h"
#include "glad/glad.h"
namespace game
{
  namespace gfx
  {
    namespace gl
    {
      // We are directly dependent on a filename here, hmm. Anyone smell that?
      Standard_Shader::Standard_Shader() noexcept
        : prog_(Program::from_files("shader/basic/v", "shader/basic/f"))
      {
        proj_loc_ = prog_.get_uniform_location("proj");
        view_loc_ = prog_.get_uniform_location("view");
        model_loc_ = prog_.get_uniform_location("model");
        diffuse_loc_ = prog_.get_uniform_location("dif");
        sampler_loc_ = prog_.get_uniform_location("tex");
      }

      void Standard_Shader::set_projection(glm::mat4 const& proj) noexcept
      {
        glUniformMatrix4fv(proj_loc_, 1, GL_FALSE, &proj[0][0]);
      }
      void Standard_Shader::set_view(glm::mat4 const& view) noexcept
      {
        glUniformMatrix4fv(view_loc_, 1, GL_FALSE, &view[0][0]);
      }
      void Standard_Shader::set_model(glm::mat4 const& model) noexcept
      {
        glUniformMatrix4fv(model_loc_, 1, GL_FALSE, &model[0][0]);
      }

      void Standard_Shader::set_texture(unsigned int tex) noexcept
      {
        glUniform1i(sampler_loc_, tex);
      }
      void Standard_Shader::set_diffuse(Color const& c) noexcept
      {
        glUniform3f(diffuse_loc_, c.r / (float) 0xff, c.g / (float) 0xff,
                    c.b / (float) 0xff);
      }
      void Standard_Shader::use() noexcept
      {
        prog_.use();
      }
    }
  }
}
