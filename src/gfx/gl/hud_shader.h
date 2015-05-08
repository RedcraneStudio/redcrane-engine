/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include <glm/glm.hpp>
#include "program.h"
#include "ishader.h"
namespace game
{
  namespace gfx
  {
    namespace gl
    {
      struct Hud_Shader : public IShader
      {
        Hud_Shader() noexcept;

        void set_texture(unsigned int tex) noexcept override;
        void set_diffuse(Color const&) noexcept override;
        void use() noexcept override;
      private:
        GLint sampler_loc_;
        GLint diffuse_loc_;
        Program prog_;
      };
    }
  }
}
