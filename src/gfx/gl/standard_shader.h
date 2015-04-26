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
      struct Standard_Shader : public IShader
      {
        Standard_Shader() noexcept;

        void set_projection(glm::mat4 const& proj) noexcept override;
        void set_view(glm::mat4 const& view) noexcept override;
        void set_model(glm::mat4 const& model) noexcept override;

        void set_texture(unsigned int tex) noexcept override;
        void set_diffuse(Color const&) noexcept override;
        void use() noexcept override;
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
