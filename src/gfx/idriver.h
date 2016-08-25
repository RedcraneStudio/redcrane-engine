/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include "../common/vec.h"
#include "itexture.h"
#include "ibuffer.h"
#include "imesh.h"
#include "ishader.h"
#include "common.h"
#include "iframebuffer.h"
namespace redc
{
  // Forward declare the color structure.
  struct Color;

  namespace gfx
  {
    //! Runtime abstraction for OpenGL (and possibly other APIs soon).
    struct IDriver
    {
      IDriver(Vec<int> size) : extents_(size) {}
      virtual ~IDriver() {}

      virtual std::unique_ptr<IBuffer> make_buffer_repr() = 0;
      virtual void make_buffers(std::size_t, std::unique_ptr<IBuffer>* bufs) = 0;

      virtual void bind_buffer(IBuffer&, Buffer_Target) = 0;

      virtual std::unique_ptr<IShader> make_shader_repr() = 0;
      virtual void make_shaders(std::size_t, std::unique_ptr<IShader>* shaders) = 0;

      virtual void use_shader(IShader&, bool force = false) = 0;
      virtual IShader* active_shader() const = 0;

      virtual std::unique_ptr<IMesh> make_mesh_repr() = 0;
      virtual void make_meshes(std::size_t, std::unique_ptr<IMesh>* meshes) = 0;

      virtual void bind_mesh(IMesh& mesh, bool force = false) = 0;
      virtual void unbind_mesh() = 0;

      virtual std::unique_ptr<ITexture> make_texture_repr() = 0;
      virtual void make_textures(std::size_t, std::unique_ptr<ITexture>* texs) = 0;

      // IDEA! Give the texture to a shader when setting a sampler and have the
      // shader bind the texture to an open slot. The slot must be coordinated
      // with the driver somehow, and I think I'll have to give the driver a bit
      // more information to make this work.
      virtual void bind_texture(ITexture& tex, Texture_Target) = 0;

      virtual void active_texture(Texture_Slot loc) = 0;
      virtual std::size_t num_texture_slots() = 0;

      virtual std::unique_ptr<IFramebuffer> make_framebuffer_repr() = 0;
      virtual void bind_framebuffer(IFramebuffer& buf, Fbo_Binding binding) = 0;

      virtual void use_framebuffer_draw_buffers(std::size_t num,
                                                Draw_Buffer* bufs) = 0;
      virtual void use_default_draw_buffers() = 0;

      virtual std::unique_ptr<IRenderbuffer> make_renderbuffer_repr() = 0;
      virtual void bind_renderbuffer(IRenderbuffer& buf) = 0;

      virtual void set_clear_color(Color const& color) = 0;
      virtual void set_clear_depth(float val) = 0;

      virtual void clear() = 0;
      virtual void clear_color() = 0;
      virtual void clear_depth() = 0;

      virtual void depth_test(bool enable) = 0;
      virtual void write_depth(bool enable) = 0;
      virtual void blending(bool enable) = 0;
      virtual void face_culling(bool enable) = 0;
      virtual void cull_side(Cull_Side side) = 0;

      virtual void set_blend_policy(Blend_Policy) = 0;

      Vec<int> window_extents() const
      { return extents_; }
      void window_extents(Vec<int> extents)
      { extents_ = extents; }

      virtual void check_error() = 0;

    private:
      Vec<int> extents_;
    };
  }
}
