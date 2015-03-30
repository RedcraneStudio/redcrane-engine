/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
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

      struct Prepared_Texture
      {
        Texture texture;
        GLuint tex_id;

        Prepared_Texture(Prepared_Texture&&) noexcept = default;
        Prepared_Texture(Prepared_Texture const&) noexcept = delete;

        Prepared_Texture& operator=(Prepared_Texture&&) noexcept = default;
        Prepared_Texture& operator=(Prepared_Texture const&) noexcept = delete;
      };

      struct Pipeline
      {
        ~Pipeline() noexcept;

        /*!
         * \brief Swallow a mesh for this instances own purposes.
         */
        Mesh* prepare_mesh(Mesh&& mesh) noexcept;
        Mesh remove_mesh(Mesh& mesh) noexcept;

        bool render_mesh(Mesh& mesh) noexcept;

        Texture* prepare_texture(Texture&& texture) noexcept;
        Texture remove_texture(Texture& texture) noexcept;

        /*!
         * \brief Binds a prepared texture to a given texture unit.
         */
        bool set_texture(Texture&, GLuint) noexcept;

        inline void clear() noexcept;
        inline void clear_color() noexcept;
        inline void clear_depth() noexcept;

      private:
        void uninit_pipeline_mesh_(Prepared_Mesh& mesh) noexcept;
        void uninit_pipeline_texture_(Prepared_Texture& tex) noexcept;

        // Maybe could expose this in the interface to return a null ptr?
        // Maybe this could be a virtual function in the ABC?
        Prepared_Mesh* find_prepared_(Mesh&) noexcept;
        Prepared_Texture* find_prepared_(Texture&) noexcept;

        std::vector<Prepared_Mesh> mesh_;
        // Use a vector of shared pointers for now so we get copy semantics.
        std::vector<std::shared_ptr<Prepared_Texture> > textures_;
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
