/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../prepared_mesh.h"
#include "glad/glad.h"
namespace strat
{
  namespace gfx
  {
    namespace gl
    {
      struct Prep_Mesh : public Prepared_Mesh
      {
        Prep_Mesh(Mesh&&) noexcept;
        // It's required by the base class that we do this.
        ~Prep_Mesh() noexcept { uninit(); }

        GLuint vao;
        GLuint vertice_buffer;
        GLuint normals_buffer;
        GLuint tex_coords_buffer;
        GLuint face_index_buffer;

      private:
        void render_() const noexcept override;
        void uninit_() noexcept override;
      };
    }
  }
}
