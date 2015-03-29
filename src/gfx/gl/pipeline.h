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
      struct Pipeline_Mesh
      {
        Mesh mesh;

        GLuint vao;
        GLuint vertice_buffer;
        GLuint normals_buffer;
        GLuint tex_coords_buffer;
        GLuint face_index_buffer;
      };

      struct Pipeline_Texture
      {
        Texture texture;

        GLuint tex_id;
      };

      struct Pipeline
      {
        /*!
         * \brief Swallow a mesh for our own purposes.
         *
         * \returns The returned pointer is valid as long as the instance that
         * created it. It is not to be deleted or otherwise uninitialized.
         */
        Pipeline_Mesh* prepare_mesh(Mesh&& mesh) noexcept;
        Mesh remove_mesh(Pipeline_Mesh& mesh) noexcept;

        void render_pipeline_mesh(Pipeline_Mesh* mesh) noexcept;

        Pipeline_Texture* prepare_texture(Texture&& texture) noexcept;
        Texture remove_texture(Pipeline_Texture& texture) noexcept;

        inline void clear() noexcept;
        inline void clear_color() noexcept;
        inline void clear_depth() noexcept;

      private:
        void uninit_pipeline_mesh_(Pipeline_Mesh& mesh) noexcept;
        void uninit_pipeline_texture_(Pipeline_Texture& texture) noexcept;

        std::vector<Pipeline_Mesh> mesh_;
        std::vector<Pipeline_Texture> textures_;
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
