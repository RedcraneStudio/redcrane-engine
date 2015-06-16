/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "mesh.h"
namespace game { namespace gfx { namespace null
{
  Null_Mesh::buf_t
  Null_Mesh::allocate_buffer(std::size_t size, Usage_Hint, Upload_Hint)
  {
    auto buf = id_gen_.get();
    push_buffer_(buf, size);
    return buf;
  }

  void Null_Mesh::reallocate_buffer(buf_t buf, std::size_t size,
                                    Usage_Hint, Upload_Hint)
  {
    set_buffer_size_(buf, size);
  }

  Null_Mesh::buf_t
  Null_Mesh::allocate_element_array(unsigned int elements, Usage_Hint,
                                    Upload_Hint)
  {
    auto buf = id_gen_.get();
    // I don't like how we are allocated using a type the user has no choice in
    // TODO: fix this in the real implementation.
    push_buffer_(buf, elements * sizeof(unsigned int));
    return buf;
  }
  void Null_Mesh::unallocate_buffer(buf_t buf) noexcept
  {
    erase_buffer_(buf);
    id_gen_.remove(buf);
  }
} } }
