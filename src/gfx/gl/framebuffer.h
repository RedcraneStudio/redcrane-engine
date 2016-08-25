/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#ifndef REDC_GFX_GL_FRAMEBUFFER_H
#define REDC_GFX_GL_FRAMEBUFFER_H
#include "driver.h"
#include "../iframebuffer.h"
namespace redc { namespace gfx { namespace gl
{
  struct GL_Renderbuffer : public IRenderbuffer
  {
    GL_Renderbuffer(Driver& driver);
    virtual ~GL_Renderbuffer();

    void reinitialize() override;

    void define_storage(Texture_Format format, Vec<std::size_t> size) override;
    void bind();

    GLuint repr;

  private:
    Driver* driver_;

    void allocate_repr_();
    void unallocate_repr_();
  };
  struct GL_Framebuffer : public IFramebuffer
  {
    GL_Framebuffer(Driver& driver);
    virtual ~GL_Framebuffer();

    void reinitialize() override;

    void attach(Attachment attach, ITexture& texture) override;
    void attach(Attachment attach, ITexture& texture,
                Cube_Map_Texture side) override;
    void attach(Attachment attach, IRenderbuffer& buf) override;

    Fbo_Status status() override;

    void bind(GLenum target);
    void use();

    GLuint repr;
  private:
    Driver* driver_;

    void attach_texture_(Attachment attach, GLenum target, GLuint tex);

    void allocate_repr_();
    void unallocate_repr_();
  };
} } }
#endif // Header Guard
