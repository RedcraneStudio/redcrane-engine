/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "../ishader.h"
#include "program.h"
namespace strat
{
  namespace gfx
  {
    namespace gl
    {
      struct Basic_Shader : public IShader
      {
        Basic_Shader() noexcept;

        void set_projection(glm::mat4 const& proj) noexcept override;
        void set_view(glm::mat4 const& view) noexcept override;
        void set_model(glm::mat4 const& model) noexcept override;

        void set_material(Material const&) noexcept override;
        void use() noexcept override;
        void render(Prepared_Mesh const&) noexcept override;
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
