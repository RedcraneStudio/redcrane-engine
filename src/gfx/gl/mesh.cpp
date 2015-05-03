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
    unallocate_();
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
  }
  void GL_Mesh::allocate(unsigned int max_verts,
                         unsigned int max_elemnt_indices, Usage_Hint uh,
                         Upload_Hint up, Primitive_Type) noexcept
  {
    // Clean the slate.
    unallocate_();

    allocate_vao_();
    allocate_buffers_();

    allocate_array_buffer_(bufs[0], 0, 3, max_verts * sizeof(glm::vec3),
                           GL_FLOAT, get_gl_hint(up, uh));
    allocate_array_buffer_(bufs[1], 1, 3, max_verts * sizeof(glm::vec3),
                           GL_FLOAT, get_gl_hint(up, uh));
    allocate_array_buffer_(bufs[2], 2, 2, max_verts * sizeof(glm::vec2),
                           GL_FLOAT, get_gl_hint(up, uh));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufs[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 max_elemnt_indices * sizeof(unsigned int), NULL,
                 get_gl_hint(up, uh));
  }
  void GL_Mesh::allocate_from(Mesh_Data const& md) noexcept
  {
    unallocate_();

    allocate_vao_();
    allocate_buffers_();

    // This is adapted from set_vertices.
    // Copy the vertex data out.
    std::vector<glm::vec3> poss;
    std::vector<glm::vec3> norms;
    std::vector<glm::vec2> uvs;
    for(auto iter = md.vertices.begin(); iter != md.vertices.end(); ++iter)
    {
      poss.push_back(iter->position);
      norms.push_back(iter->normal);
      uvs.push_back(iter->uv);
    }

    allocate_array_buffer_(bufs[0], 0, 3,
                           md.vertices.size() * sizeof(glm::vec3), GL_FLOAT,
                           get_gl_hint(md.upload_hint, md.usage_hint),
                           &poss[0]);
    allocate_array_buffer_(bufs[1], 1, 3,
                           md.vertices.size() * sizeof(glm::vec3), GL_FLOAT,
                           get_gl_hint(md.upload_hint, md.usage_hint),
                           &norms[0]);
    allocate_array_buffer_(bufs[2], 2, 2,
                           md.vertices.size() * sizeof(glm::vec2), GL_FLOAT,
                           get_gl_hint(md.upload_hint, md.usage_hint),
                           &uvs[0]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufs[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 md.elements.size() * sizeof(unsigned int), &md.elements[0],
                 get_gl_hint(md.upload_hint, md.usage_hint));

    num_indices = md.elements.size();
    primitive = get_gl_primitive(md.primitive);
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
  void GL_Mesh::set_element_indices(unsigned int b, unsigned int l,
                                    unsigned int const* i) noexcept
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufs[3]);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, b * sizeof(unsigned int),
                    l * sizeof(unsigned int), i);
  }
  void GL_Mesh::set_num_element_indices(unsigned int i) noexcept
  {
    num_indices = i;
  }
  void GL_Mesh::unallocate_() noexcept
  {
    glDeleteBuffers(bufs.size(), &bufs[0]);
    if(vao) glDeleteVertexArrays(1, &vao);
  }
  void GL_Mesh::allocate_vao_() noexcept
  {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
  }
  void GL_Mesh::allocate_buffers_() noexcept
  {
    glGenBuffers(bufs.size(), &bufs[0]);
  }
  void GL_Mesh::allocate_array_buffer_(GLuint buf, GLuint attrib_index,
                                       GLint cs, std::size_t size, GLenum type,
                                       GLenum usage) noexcept
  {
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, size, NULL, usage);
    glVertexAttribPointer(attrib_index, cs, type, GL_FALSE, 0, 0);
  }
  void GL_Mesh::allocate_array_buffer_(GLuint buf, GLuint attrib_index,
                                       GLint cs, std::size_t size, GLenum type,
                                       GLenum usage, void* data) noexcept
  {
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    glVertexAttribPointer(attrib_index, cs, type, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(attrib_index);
  }
  void GL_Mesh::bind() const noexcept
  {
    glBindVertexArray(vao);
  }
  void GL_Mesh::draw() const noexcept
  {
    glDrawElements(primitive, num_indices, GL_UNSIGNED_INT, 0);
  }
} } }
