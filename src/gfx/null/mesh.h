/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../mesh.h"
#include "../../common/id_gen.hpp"
namespace redc { namespace gfx { namespace null
{
  struct Null_Mesh : public Mesh
  {
    void make_buffers(unsigned int, buf_t*) {};
    void allocate_buffer(buf_t, Buffer_Type, unsigned int,
                         void const* const, Usage_Hint, Upload_Hint) {};

    buf_t allocate_buffer(std::size_t, Usage_Hint,Upload_Hint) override;

    void reallocate_buffer(buf_t buf, std::size_t size, Usage_Hint,
                           Upload_Hint) override;

    buf_t allocate_element_array(unsigned int elements, Usage_Hint,
                                        Upload_Hint) override;

    // These functions accept a buffer paramater which could have been created
    // as a regular buffer or an element array.
    void unallocate_buffer(buf_t) noexcept override;

    // Offset is in bytes!
    void buffer_data(buf_t, unsigned int, unsigned int,
                     void const* const) noexcept override {}

    void format_buffer(buf_t, unsigned int, unsigned short,
                       Buffer_Format, unsigned int,
                       unsigned int) noexcept override {}

    void enable_vertex_attrib(unsigned int) noexcept override {}

    void set_primitive_type(Primitive_Type) noexcept override {}

    /*
     * \brief Use the given buffer as an element array when rendering with
     * draw_elements.
     */
    void use_elements(buf_t) noexcept override {}

    void draw_arrays(unsigned int, unsigned int) noexcept override {}
    // St is given in elements, not bytes.
    void draw_elements(unsigned int, unsigned int) noexcept override {}
    void draw_elements_base_vertex(unsigned int, unsigned int,
                                   unsigned int) noexcept override {}
  private:
    ID_Gen<buf_t> id_gen_;
  };
} } }
