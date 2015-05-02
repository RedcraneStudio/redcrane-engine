/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "glad/glad.h"
//#include "../mesh.h"
namespace game
{
  namespace gfx
  {
    namespace gl
    {
      struct Prep_Mesh
      {
        //Prep_Mesh(Mesh&) noexcept;
        ~Prep_Mesh() noexcept;

        Prep_Mesh(Prep_Mesh&&) noexcept;
        Prep_Mesh& operator=(Prep_Mesh&&) noexcept;

        GLuint vao;
        GLuint vertice_buffer;
        GLuint normals_buffer;
        GLuint tex_coords_buffer;
        GLuint face_index_buffer;

        GLsizei num_faces;

        void bind() const noexcept;
        void draw() const noexcept;
      };
    }
  }
}
