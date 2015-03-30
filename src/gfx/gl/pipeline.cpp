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

        template <class T>
        struct Matches_Pointer
        {
          Matches_Pointer(T const* t) noexcept : ptr_(t) {}

          inline bool operator()(T const& t) const noexcept
          {
            return ptr_ == &t;
          }
          inline bool operator()(std::shared_ptr<T> const& t) const noexcept
          {
            return ptr_ == t.get();
          }

          T const* ptr_;
        };

        template <class T>
        inline Matches_Pointer<T> make_pointer_match(T const* t) noexcept
        {
          return Matches_Pointer<T>{t};
        }
      }

      Pipeline::~Pipeline() noexcept
      {
        for(auto& mesh : mesh_)
        {
          uninit_pipeline_mesh_(mesh);
        }
        for(auto& tex : textures_)
        {
          uninit_pipeline_texture_(*tex.get());
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
        auto prepared_mesh = find_prepared_(mesh);

        if(!prepared_mesh)
        {
          log_w("Mesh is not prepared so will not be removed");
          return Mesh{};
        }

        // First uninitialize the vao and buffers.
        uninit_pipeline_mesh_(*prepared_mesh);

        // Capture that mesh!
        Mesh&& old_mesh = std::move(prepared_mesh->mesh);;

        // Remove it from our vector.
        using std::begin; using std::end;
        auto new_end = std::find_if(begin(mesh_), end(mesh_),
                                    make_pointer_match(prepared_mesh));
        mesh_.erase(new_end, end(mesh_));

        // Move the original mesh out.
        return old_mesh;
      }
      bool Pipeline::render_mesh(Mesh& mesh) noexcept
      {
        auto prepared_mesh = find_prepared_(mesh);

        if(!prepared_mesh)
        {
          log_w("Mesh is not prepared so it will not be rendered");
          return false;
        }

        glBindVertexArray(prepared_mesh->vao);
        glDrawElements(GL_TRIANGLES, prepared_mesh->mesh.faces.size() * 3,
                       GL_UNSIGNED_INT, 0);
        return true;
      }
      Texture* Pipeline::prepare_texture(Texture&& texture) noexcept
      {
        auto pipetex = Prepared_Texture{std::move(texture), 0};

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

        textures_.push_back(
                       std::make_shared<Prepared_Texture>(std::move(pipetex)));
        return &textures_.back()->texture;
      }
      Texture Pipeline::remove_texture(Texture& texture) noexcept
      {
        auto prepared_tex = find_prepared_(texture);

        // If we don't own this texture.
        if(!prepared_tex)
        {
          // Welp, this is :/ awk.
          log_w("Texture is not prepared so it cannot be removed.");
          return Texture{};
        }

        // First uninitialize the vao and buffers.
        uninit_pipeline_texture_(*prepared_tex);

        // Capture that mesh!
        Texture old_texture = std::move(prepared_tex->texture);

        // Remove that pointer from our vector.
        using std::begin; using std::end;
        auto new_end = std::remove_if(begin(textures_), end(textures_),
                                      make_pointer_match(prepared_tex));
        textures_.erase(new_end, end(textures_));

        // Move the original mesh out.
        return std::move(old_texture);
      }

      bool Pipeline::set_texture(Texture& tex, GLuint i) noexcept
      {
        glActiveTexture(GL_TEXTURE0 + i);

        auto prepared_tex = find_prepared_(tex);
        if(!prepared_tex)
        {
          log_w("Texture is not prepared and will therefore not be set to a "
                "texture unit.");
          return false;
        }

        glBindTexture(GL_TEXTURE_2D, prepared_tex->tex_id);
        return true;
      }

      // Uninitialize a mesh as far as opengl is concerned *unprepare it*.
      void Pipeline::uninit_pipeline_mesh_(Prepared_Mesh& mesh) noexcept
      {
        glDeleteBuffers(1, &mesh.vertice_buffer);
        glDeleteBuffers(1, &mesh.normals_buffer);
        glDeleteBuffers(1, &mesh.tex_coords_buffer);
        glDeleteBuffers(1, &mesh.face_index_buffer);
        glDeleteVertexArrays(1, &mesh.vao);
      }
      void Pipeline::uninit_pipeline_texture_(Prepared_Texture& tex) noexcept
      {
        glDeleteTextures(1, &tex.tex_id);
      }

      Prepared_Mesh* Pipeline::find_prepared_(Mesh& m) noexcept
      {
        using std::begin; using std::end;
        auto mesh_find = std::find_if(begin(mesh_), end(mesh_),
        [&](auto const& val)
        {
          return &m == &val.mesh;
        });

        if(mesh_find == end(mesh_)) return nullptr;
        return &*mesh_find;
      }
      Prepared_Texture* Pipeline::find_prepared_(Texture& t) noexcept
      {
        using std::begin; using std::end;
        auto tex_find = std::find_if(begin(textures_), end(textures_),
        [&](auto const& val)
        {
          return &t == &val->texture;
        });

        if(tex_find == end(textures_)) return nullptr;
        return tex_find->get();
      }
    }
  }
}
