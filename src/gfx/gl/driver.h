/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <unordered_map>
#include "../idriver.h"
#include "glad/glad.h"
namespace redc
{
  namespace gfx
  {
    namespace gl
    {
      struct Driver;
    }
  }

  struct gfx::gl::Driver : public gfx::IDriver
  {
    Driver(Vec<int> size);
    ~Driver();

    std::unique_ptr<IBuffer> make_buffer_repr() override;
    void make_buffers(std::size_t, std::unique_ptr<IBuffer>* bufs) override;
    void bind_buffer(IBuffer& buf, Buffer_Target target) override;
    void bind_buffer(IBuffer& buf, GLenum target);

    std::unique_ptr<IShader> make_shader_repr() override;
    void make_shaders(std::size_t, std::unique_ptr<IShader>* shaders) override;

    void use_shader(IShader&, bool = false) override;
    IShader* active_shader() const override;

    std::unique_ptr<IMesh> make_mesh_repr() override;
    void make_meshes(std::size_t, std::unique_ptr<IMesh>* meshes) override;
    void bind_mesh(IMesh& mesh, bool = false) override;
    void unbind_mesh() override;

    std::unique_ptr<ITexture> make_texture_repr() override;
    void make_textures(std::size_t, std::unique_ptr<ITexture>* textures) override;
    void bind_texture(ITexture& tex, Texture_Target) override;
    void bind_texture(ITexture& tex, GLenum);

    void active_texture(Texture_Slot loc) override;
    std::size_t num_texture_slots() override;

    std::unique_ptr<IFramebuffer> make_framebuffer_repr() override;
    void bind_framebuffer(IFramebuffer& buf, Fbo_Binding binding) override;
    void bind_framebuffer(IFramebuffer& buf, GLenum binding);

    void use_framebuffer_draw_buffers(std::size_t num,
                                      Draw_Buffer* bufs) override;
    void use_default_draw_buffers() override;

    std::unique_ptr<IRenderbuffer> make_renderbuffer_repr() override;
    void bind_renderbuffer(IRenderbuffer& buf) override;

    void set_clear_color(Color const&) override;
    void set_clear_depth(float) override;

    void clear() override;
    void clear_color() override;
    void clear_depth() override;

    void depth_test(bool enable) override;
    void write_depth(bool enable) override;
    void blending(bool enable) override;
    void face_culling(bool enable) override;
    void cull_side(Cull_Side side) override;

    void set_blend_policy(Blend_Policy) override;

    void check_error() override;

  private:
    // Store state so we can avoid draw calls.
    GLenum cur_buffer_target_;
    IBuffer* cur_buffer_;

    IShader* cur_shader_;
    IMesh* cur_mesh_;
    IRenderbuffer* cur_renderbuffer_;

    GLenum cur_texture_target_;
    ITexture* cur_texture_;

    GLenum cur_fbo_target_;
    IFramebuffer* bound_framebuffer_;

    bool is_default_draw_buffers_ = true;
  };
}
