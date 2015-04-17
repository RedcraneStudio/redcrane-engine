/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "prep_mesh.h"
namespace game
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

      Prep_Mesh::Prep_Mesh(Mesh& mesh) noexcept
      {
        // Initialize the vao and buffer and such.
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Initialize the vertices.
        init_array_buffer(0, mesh.vertices, vertice_buffer, 3, GL_FLOAT,
                          GL_FALSE);
        // Initialize normals.
        if(mesh.normals.size())
        {
          init_array_buffer(1, mesh.normals, normals_buffer, 3, GL_FLOAT,
                            GL_TRUE);
        }
        // Initialize texture coordinates.
        if(mesh.tex_coords.size())
        {
          init_array_buffer(2, mesh.tex_coords, tex_coords_buffer, 2,
                            GL_FLOAT, GL_FALSE);
        }

        glGenBuffers(1, &face_index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, face_index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     mesh.faces.size() * sizeof(GLuint),
                     &mesh.faces[0], GL_STATIC_DRAW);

        glBindVertexArray(0);

        num_faces = mesh.faces.size();
      }
      Prep_Mesh::~Prep_Mesh() noexcept
      {
        if(vertice_buffer) glDeleteBuffers(1, &vertice_buffer);
        if(normals_buffer) glDeleteBuffers(1, &normals_buffer);
        if(tex_coords_buffer) glDeleteBuffers(1, &tex_coords_buffer);
        if(face_index_buffer) glDeleteBuffers(1, &face_index_buffer);
        if(vao) glDeleteVertexArrays(1, &vao);
      }

      Prep_Mesh::Prep_Mesh(Prep_Mesh&& m) noexcept
        : vao(m.vao), vertice_buffer(m.vertice_buffer),
          normals_buffer(m.normals_buffer),
          tex_coords_buffer(m.tex_coords_buffer),
          face_index_buffer(m.face_index_buffer),
          num_faces(m.num_faces)
      {
        m.vao = 0;
        m.vertice_buffer = 0;
        m.normals_buffer = 0;
        m.tex_coords_buffer = 0;
        m.face_index_buffer = 0;
      }
      Prep_Mesh& Prep_Mesh::operator=(Prep_Mesh&& m) noexcept
      {
        vao = m.vao;
        vertice_buffer = m.vertice_buffer;
        normals_buffer = m.normals_buffer;
        tex_coords_buffer = m.tex_coords_buffer;
        face_index_buffer = m.face_index_buffer;
        num_faces = m.num_faces;

        m.vao = 0;
        m.vertice_buffer = 0;
        m.normals_buffer = 0;
        m.tex_coords_buffer = 0;
        m.face_index_buffer = 0;

        return *this;
      }

      void Prep_Mesh::bind() const noexcept
      {
        glBindVertexArray(vao);
      }
      void Prep_Mesh::draw() const noexcept
      {
        glDrawElements(GL_TRIANGLES, num_faces, GL_UNSIGNED_INT, 0);
      }
    }
  }
}
