/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "mesh.h"
#include <algorithm>
#include "common.h"
#include "../../common/debugging.h"
namespace redc { namespace gfx { namespace gl
{
  GL_Mesh::GL_Mesh(Driver& driver) : vao(0), elements_type_(GL_UNSIGNED_INT),
                                     elements_size_(sizeof(unsigned int)),
                                     driver_(&driver)
  {
    allocate_vao_();
  }
  GL_Mesh::~GL_Mesh()
  {
    unallocate_vao_();
  }
  void GL_Mesh::reinitialize()
  {
    unallocate_vao_();
    allocate_vao_();
  }
  void GL_Mesh::format_buffer(IBuffer& buf,
                              Attrib_Bind loc,
                              Attrib_Type ty,
                              Data_Type format,
                              std::size_t stride,
                              std::size_t offset)
  {
    driver_->bind_mesh(*this);
    // This function always deals with arrays, never element array buffers.
    driver_->bind_buffer(buf, Buffer_Target::Array);

    GLenum gl_format = to_gl_data_type(format);

    // Count the lower nibble of ty, since it contains the amount of components
    // per vertex. We are assuming here that the upper bit is one, signifying a
    // vector (instead of a matrix), but that's fine.
    glVertexAttribPointer(loc, (int) ty & 0xf, gl_format, GL_FALSE,
                          stride, reinterpret_cast<void*>(offset));

  }

  void GL_Mesh::enable_attrib_bind(Attrib_Bind attrib)
  {
    driver_->bind_mesh(*this);
    glEnableVertexAttribArray(attrib);
  }
  void GL_Mesh::disable_attrib_bind(Attrib_Bind attrib)
  {
    driver_->bind_mesh(*this);
    glDisableVertexAttribArray(attrib);
  }

  void GL_Mesh::set_primitive_type(Primitive_Type ty)
  {
    prim_ty = ty;
  }
  Primitive_Type GL_Mesh::get_primitive_type()
  {
    return prim_ty;
  }

  /*
   * \brief Use the given buffer as an element array when rendering with
   * draw_elements.
   */
  void GL_Mesh::use_element_buffer(IBuffer& buf, Data_Type type)
  {
    // Make sure we are dealing with our own VAO, since the element array buffer
    // is part of VAO state.
    driver_->bind_mesh(*this);
    driver_->bind_buffer(buf, Buffer_Target::Element_Array);

    elements_type_ = to_gl_data_type(type);
    elements_size_ = data_type_size(type);
  }

  void GL_Mesh::draw_arrays(unsigned int st, unsigned int count)
  {
    driver_->bind_mesh(*this);

    GLenum primitive = to_gl_primitive(prim_ty);
    glDrawArrays(primitive, st, count);
  }
  void GL_Mesh::draw_elements(unsigned int st, unsigned int count)
  {
    // Possibly query the driver for the type of the element indices later on.
    driver_->bind_mesh(*this);

    GLenum primitive = to_gl_primitive(prim_ty);
    glDrawElements(primitive, count, elements_type_,
                   reinterpret_cast<void*>(st * elements_size_));
  }
  void GL_Mesh::draw_elements_base_vertex(unsigned int st, unsigned int count,
                                          unsigned int base)
  {
    driver_->bind_mesh(*this);

    GLenum primitive = to_gl_primitive(prim_ty);
    glDrawElementsBaseVertex(primitive, count, GL_UNSIGNED_INT,
                             reinterpret_cast<void*>(st*sizeof(unsigned int)),
                             base);
  }

  void GL_Mesh::unallocate_vao_()
  {
    if(vao) glDeleteVertexArrays(1, &vao);
  }
  void GL_Mesh::allocate_vao_()
  {
    glGenVertexArrays(1, &vao);
  }
  void GL_Mesh::bind()
  {
    glBindVertexArray(vao);
  }
} } }
