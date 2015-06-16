/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "glad/glad.h"
#include <array>
#include "../mesh.h"
#include "driver.h"
namespace game { namespace gfx { namespace gl
{
  struct GL_Mesh : public Mesh
  {
    GL_Mesh(Driver& driver) noexcept;
    virtual ~GL_Mesh() noexcept;

    unsigned int allocate_buffer(std::size_t size, Usage_Hint,
                                 Upload_Hint) override;
    unsigned int allocate_element_array(unsigned int elements, Usage_Hint,
                                        Upload_Hint) override;

    void reallocate_buffer(unsigned int buf, std::size_t size, Usage_Hint,
                           Upload_Hint) override;
    void unallocate_buffer(unsigned int) noexcept override;

    void buffer_data(unsigned int buf, unsigned int offset, unsigned int size,
                     void const* const d) noexcept override;

    void format_buffer(unsigned int buf,
                       unsigned int attrib,
                       unsigned short size, // must be 1 2 3 or 4
                       Buffer_Format format,
                       unsigned int stride,
                       unsigned int offset) noexcept override;
    void enable_vertex_attrib(unsigned int attrib) noexcept override;

    void set_primitive_type(Primitive_Type) noexcept override;

    void use_elements(unsigned int buf) noexcept override;

    void bind() noexcept;
    void draw_arrays(unsigned int start, unsigned int count) noexcept override;
    void draw_elements(unsigned int st, unsigned int count) noexcept override;
    void draw_elements_base_vertex(unsigned int st, unsigned int count,
                                   unsigned int base) noexcept override;

    GLuint vao;
    GLenum primitive;

  private:
    Driver* driver_;

    void unallocate_() noexcept;
    void allocate_vao_() noexcept;
  };
} } }
