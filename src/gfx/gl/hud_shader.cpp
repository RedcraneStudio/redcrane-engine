/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "hud_shader.h"
#include "glad/glad.h"
namespace game
{
  namespace gfx
  {
    namespace gl
    {
      // We are directly dependent on a filename here, hmm. Anyone smell that?
      Hud_Shader::Hud_Shader() noexcept
        : prog_(Program::from_files("shader/hud/v", "shader/hud/f"))
      {
        sampler_loc_ = prog_.get_uniform_location("tex");
        diffuse_loc_ = prog_.get_uniform_location("dif");
      }

      void Hud_Shader::set_texture(unsigned int tex) noexcept
      {
        glUniform1i(sampler_loc_, tex);
      }
      void Hud_Shader::set_diffuse(Color const& c) noexcept
      {
        glUniform4f(diffuse_loc_, c.r / (float) 0xff, c.g / (float) 0xff,
                    c.b / (float) 0xff, c.a / (float) 0xff);
      }
      void Hud_Shader::use() noexcept
      {
        prog_.use();
      }
    }
  }
}
