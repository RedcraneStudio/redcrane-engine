 /*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#ifndef REDC_GFX_GL_BUFFER_H
#define REDC_GFX_GL_BUFFER_H
#include "driver.h"
#include "../ibuffer.h"
namespace redc { namespace gfx { namespace gl
{
  struct GL_Buffer : public IBuffer
  {
    GL_Buffer(Driver& driver);
    virtual ~GL_Buffer();

    void allocate(Buffer_Target, std::size_t size, const void* data,
                  Usage_Hint, Upload_Hint) override;

    std::size_t allocated_size() override { return size_; }

    // Buffer must have been allocated before this.
    void update(std::size_t offset, std::size_t size,
                const void* data) override;

    void reinitialize() override;
    void bind(GLenum target);
  private:
    void allocate_buf_();
    void unallocate_buf_();

    Driver* driver_;

    std::size_t size_;

    GLuint repr;
  };
} } }
#endif // REDC_GFX_GL_BUFFER_H
