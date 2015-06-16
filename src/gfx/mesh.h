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
  enum class Primitive_Type
  {
    Triangle, Line
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

    virtual unsigned int allocate_buffer(std::size_t size,
                                         Usage_Hint, Upload_Hint) = 0;
    virtual void reallocate_buffer(unsigned int buf,
                                   std::size_t size,
                                   Usage_Hint, Upload_Hint) = 0;

    virtual unsigned int allocate_element_array(unsigned int elements,
                                                Usage_Hint, Upload_Hint) = 0;

    // These functions accept a buffer paramater which could have been created
    // as a regular buffer or an element array.
    virtual void unallocate_buffer(unsigned int) noexcept = 0;

    // Offset is in bytes!
    virtual void buffer_data(unsigned int, unsigned int off, unsigned int size,
                             void const* const data) noexcept = 0;

    unsigned int get_num_allocated_buffers() noexcept;
    unsigned int get_buffer(unsigned int index) noexcept;
    unsigned int get_buffer_size(unsigned int buf) noexcept;

    virtual void format_buffer(unsigned int buf,
                               unsigned int attrib,
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
    virtual void use_elements(unsigned int buf) noexcept = 0;

    virtual void draw_arrays(unsigned int start, unsigned int c) noexcept = 0;
    // St is given in elements, not bytes.
    virtual void draw_elements(unsigned int st, unsigned int c) noexcept = 0;
    virtual void draw_elements_base_vertex(unsigned int st, unsigned int c,
                                           unsigned int bv) noexcept = 0;

  protected:
    void push_buffer_(unsigned int buf, unsigned int bytes) noexcept;
    void erase_buffer_(unsigned int buf) noexcept;
    void set_buffer_size_(unsigned int buf, unsigned int bytes) noexcept;
  private:
    // Index: Buf + Size
    std::vector<std::tuple<unsigned int, unsigned int> > bufs_;
  };

  template <class T>
  bool mesh_has_room(Mesh& m, unsigned int buffer, unsigned int offset,
                     unsigned int elements) noexcept
  {
    return m.get_buffer_size(buffer) - offset >= elements * sizeof(T);
  }
}
