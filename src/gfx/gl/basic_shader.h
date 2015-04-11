/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include <glm/glm.hpp>
#include "../../common/color.h"
#include "program.h"
namespace strat
{
  namespace gfx
  {
    namespace gl
    {
      struct Basic_Shader
      {
        Basic_Shader() noexcept;

        void set_projection(glm::mat4 const& proj) noexcept;
        void set_view(glm::mat4 const& view) noexcept;
        void set_model(glm::mat4 const& model) noexcept;

        void set_texture(unsigned int tex) noexcept;
        void set_diffuse(Color const&) noexcept;
        void use() noexcept;
      private:
        GLint proj_loc_;
        GLint view_loc_;
        GLint model_loc_;
        GLint diffuse_loc_;
        GLint sampler_loc_;

        Program prog_;
      };
    }
  }
}
