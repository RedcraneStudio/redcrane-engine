/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "mesh.h"
#include <algorithm>
namespace game { namespace gfx { namespace gl
{
  GL_Mesh::~GL_Mesh() noexcept
  {
    glDeleteBuffers(bufs.size(), &bufs[0]);
    glDeleteVertexArrays(1, &vao);
  }
  namespace
  {
    GLenum get_gl_hint(Upload_Hint up_h, Usage_Hint us_h) noexcept
    {
      if(up_h == Upload_Hint::Static && us_h == Usage_Hint::Draw)
      { return GL_STATIC_DRAW; }
      else if(up_h == Upload_Hint::Static && us_h == Usage_Hint::Read)
      { return GL_STATIC_READ; }
      else if(up_h == Upload_Hint::Static && us_h == Usage_Hint::Copy)
      { return GL_STATIC_COPY; }
      else if(up_h == Upload_Hint::Dynamic && us_h == Usage_Hint::Draw)
      { return GL_DYNAMIC_DRAW; }
      else if(up_h == Upload_Hint::Dynamic && us_h == Usage_Hint::Read)
      { return GL_DYNAMIC_READ; }
      else if(up_h == Upload_Hint::Dynamic && us_h == Usage_Hint::Copy)
      { return GL_DYNAMIC_COPY; }
      else if(up_h == Upload_Hint::Stream && us_h == Usage_Hint::Draw)
      { return GL_STREAM_DRAW; }
      else if(up_h == Upload_Hint::Stream && us_h == Usage_Hint::Read)
      { return GL_STREAM_READ; }
      else if(up_h == Upload_Hint::Stream && us_h == Usage_Hint::Copy)
      { return GL_STREAM_COPY; }

      // This shouldn't happen at all, but just in case we pick a reasonable
      // default.
      return GL_DYNAMIC_DRAW;
    }

    GLenum get_gl_primitive(Primitive_Type type) noexcept
    {
      switch(type)
      {
        case Primitive_Type::Line:
          return GL_LINES;
        case Primitive_Type::Triangle:
          return GL_TRIANGLES;
      }
    }

    template <class T>
    std::vector<T> transform_vertex(Mesh_Data const& m, T Vertex::* c) noexcept
    {
      using std::begin; using std::end;

      auto ret = std::vector<T>{};
      ret.resize(m.vertices.size());

      std::transform(begin(m.vertices), end(m.vertices),
                     begin(ret),
      [&](auto const& vertex)
      {
        return vertex.*c;
      });
      return ret;
    }

    template <class T>
    void init_array_buffer(GLuint attrib,
                           std::vector<T> const& buf,
                           GLuint& buf_value, GLenum usage,
                           GLint size, GLenum type,
                           bool normalized) noexcept
    {
      glBindBuffer(GL_ARRAY_BUFFER, buf_value);
      glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(T),
                   &buf[0], usage);
      glVertexAttribPointer(attrib, size, type, normalized, 0, 0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glEnableVertexAttribArray(attrib);
    }
  }
  void GL_Mesh::prepare(Mesh_Data const& data) noexcept
  {
    // For write now, have a vao per object.
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(bufs.size(), &bufs[0]);

    auto positions = transform_vertex(data, &Vertex::position);
    init_array_buffer(0, positions, bufs[0],
                      get_gl_hint(data.upload_hint, data.usage_hint),
                      3, GL_FLOAT, GL_FALSE);
    auto normals = transform_vertex(data, &Vertex::normal);
    init_array_buffer(1, normals, bufs[1],
                      get_gl_hint(data.upload_hint, data.usage_hint),
                      3, GL_FLOAT, GL_FALSE);
    auto uvs = transform_vertex(data, &Vertex::uv);
    init_array_buffer(2, uvs, bufs[2],
                      get_gl_hint(data.upload_hint, data.usage_hint),
                      2, GL_FLOAT, GL_FALSE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufs[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 data.elements.size() * sizeof(unsigned int),
                 &data.elements[0],
                 get_gl_hint(data.upload_hint, data.usage_hint));
    num_faces = data.elements.size();
    primitive = get_gl_primitive(data.primitive);
  }
  void GL_Mesh::set_vertices(unsigned int b, unsigned int l,
                             Vertex const* v) noexcept
  {
    // Copy the vertex data out.
    std::vector<glm::vec3> poss;
    std::vector<glm::vec3> norms;
    std::vector<glm::vec2> uvs;
    for(auto iter = v; iter != v + l; ++iter)
    {
      poss.push_back(iter->position);
      norms.push_back(iter->normal);
      uvs.push_back(iter->uv);
    }

    // Update the buffer data.
    // @Optimization don't upload data that hasn't been changed.

    glBindBuffer(GL_ARRAY_BUFFER, bufs[0]);
    glBufferSubData(GL_ARRAY_BUFFER, b * sizeof(glm::vec3),
                    l * sizeof(glm::vec3), &poss[0]);

    glBindBuffer(GL_ARRAY_BUFFER, bufs[1]);
    glBufferSubData(GL_ARRAY_BUFFER, b * sizeof(glm::vec3),
                    l * sizeof(glm::vec3), &norms[0]);

    glBindBuffer(GL_ARRAY_BUFFER, bufs[2]);
    glBufferSubData(GL_ARRAY_BUFFER, b * sizeof(glm::vec2),
                    l * sizeof(glm::vec2), &uvs[0]);
  }
  void GL_Mesh::bind() const noexcept
  {
    glBindVertexArray(vao);
  }
  void GL_Mesh::draw() const noexcept
  {
    glDrawElements(primitive, num_faces, GL_UNSIGNED_INT, 0);
  }
} } }
