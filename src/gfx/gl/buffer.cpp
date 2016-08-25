 /*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#include "buffer.h"
#include "common.h"
namespace redc { namespace gfx { namespace gl
{
  void GL_Buffer::allocate(Buffer_Target target, std::size_t size,
                           const void* data, Usage_Hint us, Upload_Hint up)
  {
    // Find the target here so we don't have to figure it out again later.
    // This functionality is provided by an overload in GL_Driver

    if(target == Buffer_Target::Element_Array)
    {
      // The element array is part of the VAO state so unbind the element array
      driver_->unbind_mesh();
    }
    GLenum gl_target = to_gl_buffer_target(target);
    driver_->bind_buffer(*this, gl_target);

    GLenum hint = to_gl_usage_hint(up, us);
    glBufferData(gl_target, size, data, hint);

    size_ = size;
  }

    // Buffer must have been allocated before this.
  void GL_Buffer::update(std::size_t offset, std::size_t size, const void* data)
  {
    driver_->bind_buffer(*this, Buffer_Target::Array);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
  }

  GL_Buffer::GL_Buffer(Driver& driver) : driver_(&driver), size_(0), repr(0)
  { allocate_buf_(); }
  GL_Buffer::~GL_Buffer() { unallocate_buf_(); }

  void GL_Buffer::reinitialize()
  {
    allocate_buf_();
    unallocate_buf_();
  }
  void GL_Buffer::bind(GLenum target)
  {
    glBindBuffer(target, repr);
  }
  void GL_Buffer::allocate_buf_()
  {
    glGenBuffers(1, &repr);

    // New buffer, new chance bb
    size_ = 0;
  }
  void GL_Buffer::unallocate_buf_()
  {
    if(repr) glDeleteBuffers(1, &repr);
  }
} } }
