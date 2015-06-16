/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "mesh.h"
#include <algorithm>
#include "common.h"
namespace game { namespace gfx { namespace gl
{
  GL_Mesh::GL_Mesh(Driver& driver) noexcept : driver_(&driver)
  {
    allocate_vao_();
  }
  GL_Mesh::~GL_Mesh() noexcept
  {
    unallocate_();
  }

  unsigned int GL_Mesh::allocate_buffer(std::size_t size, Usage_Hint us,
                                        Upload_Hint up)
  {
    if(size == 0) return 0;

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, size, NULL, get_gl_hint(up, us));

    push_buffer_(buf, size);
    return buf;
  }
  unsigned int GL_Mesh::allocate_element_array(unsigned int elements,
                                               Usage_Hint us, Upload_Hint up)
  {
    if(elements == 0) return 0;

    // Make sure our VAO is the current one before we go binding random shit to
    // the ELEMENT_ARRAY_BUFFER.
    driver_->bind_mesh(*this);

    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf);

    std::size_t size = sizeof(unsigned int) * elements;

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, NULL, get_gl_hint(up, us));
    push_buffer_(buf, size);

    return buf;
  }

  void GL_Mesh::reallocate_buffer(unsigned int buf, std::size_t size,
                                  Usage_Hint us, Upload_Hint up)
  {
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, size, NULL, get_gl_hint(up, us));

    set_buffer_size_(buf, size);
  }

  void GL_Mesh::unallocate_buffer(unsigned int buf) noexcept
  {
    glDeleteBuffers(1, &buf);
    erase_buffer_(buf);
  }

  void GL_Mesh::buffer_data(unsigned int buf, unsigned int offset,
                            unsigned int size, void const* const data) noexcept
  {
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
  }

  void GL_Mesh::format_buffer(unsigned int buf,
                              unsigned int attrib,
                              unsigned short size,
                              Buffer_Format format,
                              unsigned int stride,
                              unsigned int offset) noexcept
  {
    driver_->bind_mesh(*this);

    glBindBuffer(GL_ARRAY_BUFFER, buf);

    glVertexAttribPointer(attrib, size, get_gl_buffer_format(format),
                          GL_FALSE, stride, reinterpret_cast<void*>(offset));
  }
  void GL_Mesh::set_primitive_type(Primitive_Type prim) noexcept
  {
    primitive = get_gl_primitive(prim);
  }
  void GL_Mesh::enable_vertex_attrib(unsigned int attrib) noexcept
  {
    driver_->bind_mesh(*this);
    glEnableVertexAttribArray(attrib);
  }

  void GL_Mesh::use_elements(unsigned int buf) noexcept
  {
    // Make sure we are dealing with our own VAO.
    driver_->bind_mesh(*this);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf);

  }

  void GL_Mesh::draw_arrays(unsigned int st, unsigned int count) noexcept
  {
    driver_->bind_mesh(*this);
    glDrawArrays(primitive, st, count);
  }
  void GL_Mesh::draw_elements(unsigned int st, unsigned int count) noexcept
  {
    // Possibly query the driver for the type of the element indices later on.
    driver_->bind_mesh(*this);
    glDrawElements(primitive, count, GL_UNSIGNED_INT,
                   reinterpret_cast<void*>(st * sizeof(unsigned int)));
  }
  void GL_Mesh::draw_elements_base_vertex(unsigned int st, unsigned int count,
                                          unsigned int base) noexcept
  {
    driver_->bind_mesh(*this);
    glDrawElementsBaseVertex(primitive, count, GL_UNSIGNED_INT,
                             reinterpret_cast<void*>(st*sizeof(unsigned int)),
                             base);
  }

  void GL_Mesh::unallocate_() noexcept
  {
    std::vector<unsigned int> bufs;
    for(unsigned int i = 0; i < get_num_allocated_buffers(); ++i)
    {
      auto buf = get_buffer(i);
      bufs.push_back(buf);

      // We don't have to worry about erasing it from the buffer list because
      // we know this function will only be called on destruction and not by
      // the user.
      //erase_buffer_(buf);
    }
    glDeleteBuffers(bufs.size(), &bufs[0]);

    if(vao) glDeleteVertexArrays(1, &vao);
  }
  void GL_Mesh::allocate_vao_() noexcept
  {
    glGenVertexArrays(1, &vao);
  }
  void GL_Mesh::bind() noexcept
  {
    glBindVertexArray(vao);
  }
} } }
