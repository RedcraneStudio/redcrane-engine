/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "glad/glad.h"
#include "../../mesh.h"
#include "../../texture.h"
namespace survive
{
  namespace gfx
  {
    namespace gl
    {
      struct Prepared_Mesh
      {
        Mesh mesh;

        GLuint vao;
        GLuint vertice_buffer;
        GLuint normals_buffer;
        GLuint tex_coords_buffer;
        GLuint face_index_buffer;
      };

      struct Pipeline
      {
        /*!
         * \brief Swallow a mesh for this instances own purposes.
         */
        Mesh* prepare_mesh(Mesh&& mesh) noexcept;
        Mesh remove_mesh(Mesh& mesh) noexcept;

        bool render_mesh(Mesh& mesh) noexcept;

        inline void clear() noexcept;
        inline void clear_color() noexcept;
        inline void clear_depth() noexcept;

      private:
        void uninit_pipeline_mesh_(Prepared_Mesh& mesh) noexcept;

        std::vector<Prepared_Mesh> mesh_;
      };

      inline void Pipeline::clear() noexcept
      {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      }
      inline void Pipeline::clear_color() noexcept
      {
        glClear(GL_COLOR_BUFFER_BIT);
      }
      inline void Pipeline::clear_depth() noexcept
      {
        glClear(GL_DEPTH_BUFFER_BIT);
      }
    }
  }
}
