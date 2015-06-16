/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../mesh.h"
#include "../../common/ID_Gen.hpp"
namespace game { namespace gfx { namespace null
{
  struct Null_Mesh : public Mesh
  {
    // TODO: using buf_t = unsigned int in the Mesh ABC.
    unsigned int allocate_buffer(std::size_t, Usage_Hint,Upload_Hint) override;

    void reallocate_buffer(unsigned int buf, std::size_t size, Usage_Hint,
                           Upload_Hint) override;

    unsigned int allocate_element_array(unsigned int elements, Usage_Hint,
                                        Upload_Hint) override;

    // These functions accept a buffer paramater which could have been created
    // as a regular buffer or an element array.
    void unallocate_buffer(unsigned int) noexcept override;

    // Offset is in bytes!
    void buffer_data(unsigned int, unsigned int, unsigned int,
                     void const* const) noexcept override {}

    void format_buffer(unsigned int, unsigned int, unsigned short,
                       Buffer_Format, unsigned int,
                       unsigned int) noexcept override {}

    void enable_vertex_attrib(unsigned int) noexcept override {}

    void set_primitive_type(Primitive_Type) noexcept override {}

    /*
     * \brief Use the given buffer as an element array when rendering with
     * draw_elements.
     */
    void use_elements(unsigned int) noexcept override {}

    void draw_arrays(unsigned int, unsigned int) noexcept override {}
    // St is given in elements, not bytes.
    void draw_elements(unsigned int, unsigned int) noexcept override {}
    void draw_elements_base_vertex(unsigned int, unsigned int,
                                   unsigned int) noexcept override {}
  private:
    ID_Gen<unsigned int> id_gen_;
  };
} } }
