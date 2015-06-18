/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 *
 * This file defines the abstract class for a mesh to be implemented by
 * drivers.
 */
#pragma once
#include <array>
#include <vector>
#include <tuple>
#include "primitive_type.h"
namespace game
{
  enum class Usage_Hint
  {
    Draw, Read, Copy
  };
  enum class Upload_Hint
  {
    Static, Dynamic, Stream
  };

  enum class Buffer_Format
  {
    Float,
    Unsigned_Int,
    Short
  };

  /*!
   * \brief A simple collection of buffers for vertex attributes.
   */
  struct Mesh
  {
    virtual ~Mesh() noexcept {}

    using buf_t = unsigned int;

    virtual buf_t allocate_buffer(std::size_t sz, Usage_Hint, Upload_Hint) = 0;
    virtual void reallocate_buffer(buf_t, std::size_t size, Usage_Hint,
                                   Upload_Hint) = 0;

    virtual buf_t allocate_element_array(unsigned int elements,
                                         Usage_Hint, Upload_Hint) = 0;

    // These functions accept a buffer paramater which could have been created
    // as a regular buffer or an element array.
    virtual void unallocate_buffer(buf_t) noexcept = 0;

    // Offset is in bytes!
    virtual void buffer_data(buf_t, unsigned int off, unsigned int size,
                             void const* const data) noexcept = 0;

    unsigned int get_num_allocated_buffers() noexcept;
    buf_t get_buffer(unsigned int index) noexcept;
    unsigned int get_buffer_size(buf_t) noexcept;

    virtual void format_buffer(buf_t, unsigned int attrib,
                               unsigned short size, // must be 1 2 3 or 4
                               Buffer_Format format,
                               unsigned int stride,
                               unsigned int offset) noexcept = 0;

    virtual void enable_vertex_attrib(unsigned int attrib) noexcept = 0;

    virtual void set_primitive_type(Primitive_Type) noexcept = 0;

    /*
     * \brief Use the given buffer as an element array when rendering with
     * draw_elements.
     */
    virtual void use_elements(buf_t) noexcept = 0;

    virtual void draw_arrays(unsigned int start, unsigned int c) noexcept = 0;
    // St is given in elements, not bytes.
    virtual void draw_elements(unsigned int st, unsigned int c) noexcept = 0;
    virtual void draw_elements_base_vertex(unsigned int st, unsigned int c,
                                           unsigned int bv) noexcept = 0;

  protected:
    void push_buffer_(buf_t buf, unsigned int bytes) noexcept;
    void erase_buffer_(buf_t buf) noexcept;
    void set_buffer_size_(buf_t buf, unsigned int bytes) noexcept;
  private:
    // Index: Buf + Size
    std::vector<std::tuple<buf_t, unsigned int> > bufs_;
  };

  template <class T>
  bool mesh_has_room(Mesh& m, Mesh::buf_t buffer, unsigned int offset,
                     unsigned int elements) noexcept
  {
    return m.get_buffer_size(buffer) - offset >= elements * sizeof(T);
  }
}
