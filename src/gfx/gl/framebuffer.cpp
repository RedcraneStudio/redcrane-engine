/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "framebuffer.h"
#include "texture.h"
#include "common.h"
namespace redc { namespace gfx { namespace gl
{
  GL_Renderbuffer::GL_Renderbuffer(Driver& driver) : driver_(&driver)
  {
    allocate_repr_();
  }
  GL_Renderbuffer::~GL_Renderbuffer()
  {
    unallocate_repr_();
  }
  void GL_Renderbuffer::reinitialize()
  {
    unallocate_repr_();
    allocate_repr_();
  }

  void GL_Renderbuffer::allocate_repr_()
  {
    glGenRenderbuffers(1, &repr);
  }
  void GL_Renderbuffer::unallocate_repr_()
  {
    if(repr) glDeleteRenderbuffers(1, &repr);
  }

  void GL_Renderbuffer::define_storage(Texture_Format format,
                                       Vec<std::size_t> size)
  {
    driver_->bind_renderbuffer(*this);
    GLenum gl_format = to_gl_texture_format(format);
    glRenderbufferStorage(GL_RENDERBUFFER, gl_format, size.x, size.y);
  }
  void GL_Renderbuffer::bind()
  {
    glBindRenderbuffer(GL_RENDERBUFFER, repr);
  }

  GL_Framebuffer::GL_Framebuffer(Driver& driver) : driver_(&driver)
  {
    allocate_repr_();
  }
  GL_Framebuffer::~GL_Framebuffer()
  {
    unallocate_repr_();
  }

  void GL_Framebuffer::reinitialize()
  {
    unallocate_repr_();
    allocate_repr_();
  }

  void GL_Framebuffer::attach_texture_(Attachment attach, GLenum target, GLuint tex)
  {
    driver_->bind_framebuffer(*this, GL_DRAW_FRAMEBUFFER);

    GLenum gl_attach = to_gl_attachment(attach);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, gl_attach, target, tex, 0);
  }

  void GL_Framebuffer::attach(Attachment attach, ITexture& texture)
  {
    // Use the right texture target
    GL_Texture* gl_tex = (GL_Texture*) &texture;
    attach_texture_(attach, gl_tex->gl_target, gl_tex->tex);
  }

  void GL_Framebuffer::attach(Attachment attach, ITexture& texture,
                              Cube_Map_Texture side)
  {
    GLenum cube_map_side = to_gl_cube_map_texture(side);
    GL_Texture* gl_tex = (GL_Texture*) &texture;
    attach_texture_(attach, cube_map_side, gl_tex->tex);
  }

  void GL_Framebuffer::attach(Attachment attach, IRenderbuffer& buf)
  {
    driver_->bind_framebuffer(*this, GL_DRAW_FRAMEBUFFER);

    GL_Renderbuffer* rb = (GL_Renderbuffer*) &buf;

    GLenum gl_attach = to_gl_attachment(attach);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, gl_attach, GL_RENDERBUFFER,
                              rb->repr);
  }

  Fbo_Status GL_Framebuffer::status()
  {
    driver_->bind_framebuffer(*this, GL_DRAW_FRAMEBUFFER);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    return from_gl_fbo_status(status);
  }

  void GL_Framebuffer::bind(GLenum target)
  {
    glBindFramebuffer(target, repr);
  }

  void GL_Framebuffer::allocate_repr_()
  {
    glGenFramebuffers(1, &repr);
  }
  void GL_Framebuffer::unallocate_repr_()
  {
    if(repr) glDeleteFramebuffers(1, &repr);
  }

} } }
