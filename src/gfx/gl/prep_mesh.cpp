/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "prep_mesh.h"
namespace strat
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

      Prep_Mesh::Prep_Mesh(Mesh&& tmp_mesh) noexcept
        : Prepared_Mesh(std::move(tmp_mesh))
      {
        // Initialize the vao and buffer and such.
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Initialize the vertices.
        init_array_buffer(0, mesh().vertices, vertice_buffer,
                          3, GL_FLOAT, GL_FALSE);
        // Initialize normals.
        if(mesh().normals.size())
        {
          init_array_buffer(1, mesh().normals, normals_buffer, 3, GL_FLOAT,
                            GL_TRUE);
        }
        // Initialize texture coordinates.
        if(mesh().tex_coords.size())
        {
          init_array_buffer(2, mesh().tex_coords, tex_coords_buffer, 2,
                            GL_FLOAT, GL_FALSE);
        }

        glGenBuffers(1, &face_index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, face_index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     mesh().faces.size() * sizeof(GLuint),
                     &mesh().faces[0], GL_STATIC_DRAW);

        glBindVertexArray(0);
      }

      void Prep_Mesh::render_() const noexcept
      {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, mesh().faces.size() * 3,
                       GL_UNSIGNED_INT, 0);
      }
      void Prep_Mesh::uninit_() noexcept
      {
        glDeleteBuffers(1, &vertice_buffer);
        glDeleteBuffers(1, &normals_buffer);
        glDeleteBuffers(1, &tex_coords_buffer);
        glDeleteBuffers(1, &face_index_buffer);
        glDeleteVertexArrays(1, &vao);
      }
    }
  }
}
