/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <array>
#include "../imesh.h"
#include "driver.h"
namespace redc { namespace gfx { namespace gl
{
  struct GL_Mesh : public IMesh
  {
    GL_Mesh(Driver& driver);
    virtual ~GL_Mesh();

    void reinitialize() override;

    void format_buffer(IBuffer& buf,
                       Attrib_Bind loc,
                       Attrib_Type type,
                       Data_Type format,
                       std::size_t stride,
                       std::size_t offset) override;

    void enable_attrib_bind(Attrib_Bind attrib) override;
    void disable_attrib_bind(Attrib_Bind attrib) override;

    void set_primitive_type(Primitive_Type) override;
    Primitive_Type get_primitive_type() override;

    /*
     * \brief Use the given buffer as an element array when rendering with
     * draw_elements.
     */
    void use_element_buffer(IBuffer& buf, Data_Type dtype) override;

    void draw_arrays(unsigned int start, unsigned int c) override;
    /*!
     * \param st Start index; given in elements, not bytes.
     */
    void draw_elements(unsigned int st, unsigned int c) override;
    void draw_elements_base_vertex(unsigned int st, unsigned int c,
                                           unsigned int bv) override;

    GLuint vao;
    Primitive_Type prim_ty;

    GLenum elements_type_;
    std::size_t elements_size_;

    void bind();

  private:
    Driver* driver_;

    void unallocate_vao_();
    void allocate_vao_();
  };
} } }
