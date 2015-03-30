/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "pipeline.h"
#include <algorithm>
#include "../../common/log.h"
namespace survive
{
  namespace gfx
  {
    namespace gl
    {
      namespace
      {
        template <class T>
        void init_array_buffer(GLuint attrib,
                               std::vector<T> const& buf,
                               GLuint& buf_value,
                               GLint size, GLenum type,
                               bool normalized) noexcept
        {
          glGenBuffers(1, &buf_value);
          glBindBuffer(GL_ARRAY_BUFFER, buf_value);
          glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(T),
                       &buf[0], GL_STATIC_DRAW);
          glVertexAttribPointer(attrib, size, type, normalized, 0, 0);
          glBindBuffer(GL_ARRAY_BUFFER, 0);
          glEnableVertexAttribArray(attrib);
        }
      }

      /*!
       * Swallow a mesh for our own purposes.
       */
      Mesh* Pipeline::prepare_mesh(Mesh&& mesh) noexcept
      {
        // Make a "prepared mesh" or rather something that will store some
        // useful things about the mesh for the purposes of rendering it with
        // opengl.
        auto pipemesh = Prepared_Mesh{std::move(mesh), 0,0,0,0,0};

        // Initialize the vao and buffer and such.
        glGenVertexArrays(1, &pipemesh.vao);
        glBindVertexArray(pipemesh.vao);

        // Initialize the vertices.
        init_array_buffer(0, pipemesh.mesh.vertices, pipemesh.vertice_buffer,
                          3, GL_FLOAT, GL_FALSE);
        // Initialize normals.
        if(pipemesh.mesh.normals.size())
        {
          init_array_buffer(1, pipemesh.mesh.normals, pipemesh.normals_buffer,
                            3, GL_FLOAT, GL_TRUE);
        }
        // Initialize texture coordinates.
        if(pipemesh.mesh.tex_coords.size())
        {
          init_array_buffer(2, pipemesh.mesh.tex_coords,
                            pipemesh.tex_coords_buffer, 2, GL_FLOAT, GL_FALSE);
        }

        glGenBuffers(1, &pipemesh.face_index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pipemesh.face_index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     pipemesh.mesh.faces.size() * sizeof(GLuint),
                     &pipemesh.mesh.faces[0], GL_STATIC_DRAW);

        glBindVertexArray(0);

        // Move *that* into the vector.
        mesh_.push_back(std::move(pipemesh));

        // Now that we have a permanent spot for it. Give it back to the
        // client.
        return &mesh_.back().mesh;
      }
      Mesh Pipeline::remove_mesh(Mesh& mesh) noexcept
      {
        using std::begin; using std::end;
        auto new_end = std::remove_if(begin(mesh_), end(mesh_),
        [&mesh](auto& val)
        {
          // Check to see which element the pointer is pointing to.
          return &mesh == &val.mesh;
        });

        if(new_end != end(mesh_))
        {
          log_w("Mesh is not prepared so will not be removed");
          return Mesh{};
        }

        // First uninitialize the vao and buffers.
        uninit_pipeline_mesh_(*new_end);

        // Capture that mesh!
        Mesh&& old_mesh = std::move(mesh);
        // Remove it from our vector.
        mesh_.erase(new_end, end(mesh_));

        // Move the original mesh out.
        return old_mesh;
      }
      bool Pipeline::render_mesh(Mesh& mesh) noexcept
      {
        using std::begin; using std::end;
        auto prepared_mesh = std::find_if(begin(mesh_), end(mesh_),
        [&](auto const& val)
        {
          return &val.mesh == &mesh;
        });

        if(prepared_mesh == end(mesh_))
        {
          log_w("Mesh is not prepared so it will not be rendered");
          return false;
        }

        glBindVertexArray(prepared_mesh->vao);
        glDrawElements(GL_TRIANGLES, prepared_mesh->mesh.faces.size() * 3,
                       GL_UNSIGNED_INT, 0);
        return true;
      }
#if 0
      Pipeline_Texture* Pipeline::prepare_texture(Texture&& texture) noexcept
      {
        auto pipetex = Pipeline_Texture{std::move(texture), 0};

        glGenTextures(1, &pipetex.tex_id);

        // Allow for different texture units.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pipetex.tex_id);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pipetex.texture.w,
                     pipetex.texture.h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     pipetex.texture.data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

        textures_.push_back(std::move(pipetex));
        return &textures_.back();
      }
      Texture Pipeline::remove_texture(Pipeline_Texture& texture) noexcept
      {
        using std::begin; using std::end;
        auto new_end = std::remove_if(begin(textures_), end(textures_),
        [&texture](auto const& val)
        {
          // Check to see which element the pointer is pointing to.
          return &texture == &val;
        });

        // First uninitialize the vao and buffers.
        uninit_pipeline_texture_(texture);

        // Capture that mesh!
        Texture old_texture_data = std::move(texture.texture);
        // Remove it from our vector.
        textures_.erase(new_end, end(textures_));

        // Move the original mesh out.
        return std::move(old_texture_data);
      }
#endif
      // Uninitialize a mesh as far as opengl is concerned *unprepare it*.
      void Pipeline::uninit_pipeline_mesh_(Prepared_Mesh& mesh) noexcept
      {
        glDeleteBuffers(1, &mesh.vertice_buffer);
        glDeleteBuffers(1, &mesh.normals_buffer);
        glDeleteBuffers(1, &mesh.tex_coords_buffer);
        glDeleteBuffers(1, &mesh.face_index_buffer);
        glDeleteVertexArrays(1, &mesh.vao);
      }
#if 0
      void Pipeline::uninit_pipeline_texture_(Pipeline_Texture& texture) noexcept
      {
        glDeleteTextures(1, &texture.tex_id);
      }
#endif
    }
  }
}
