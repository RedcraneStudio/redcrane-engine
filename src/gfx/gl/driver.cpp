/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "driver.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"
#include "buffer.h"
#include "framebuffer.h"

#include "common.h"

#include "../../common/color.h"
#include "../../common/log.h"

#include "../../common/debugging.h"
#ifdef GAME_DEBUGGING_ENABLED
#define CAST_PTR dynamic_cast
#else
#define CAST_PTR reinterpret_cast
#endif

namespace redc
{
  namespace gfx
  {
    namespace gl
    {
      Driver::Driver(Vec<int> size) : IDriver(size)
      {
        glViewport(0, 0, size.x, size.y);
        set_blend_policy(Blend_Policy::Transparency);

        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        glEnable(GL_FRAMEBUFFER_SRGB);
        glDepthFunc(GL_LESS);

        // This solves bad texture upload with non-power-of-two sized textures.
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        depth_test(true);
        write_depth(true);
      }
      Driver::~Driver()
      {
        glUseProgram(0);
      }

      std::unique_ptr<IBuffer> Driver::make_buffer_repr()
      {
        return std::make_unique<GL_Buffer>(*this);
      }
      void Driver::make_buffers(std::size_t num, std::unique_ptr<IBuffer>* buffers)
      {
        for(std::size_t i = 0; i < num; ++i)
        {
          buffers[i] = make_buffer_repr();
        }
      }
      void Driver::bind_buffer(IBuffer& buf, Buffer_Target target)
      {
        GLenum gl_target = to_gl_buffer_target(target);
        this->bind_buffer(buf, gl_target);
      }
      void Driver::bind_buffer(IBuffer& buf, GLenum target)
      {
        if(cur_buffer_ == &buf && cur_buffer_target_ == target) return;

        GL_Buffer* buffer_ptr = CAST_PTR<GL_Buffer*>(&buf);
        REDC_ASSERT_MSG(buffer_ptr, "Buffer not make with this driver;"
                        " something is very wrong.");

        buffer_ptr->bind(target);

        cur_buffer_target_ = target;
        cur_buffer_ = buffer_ptr;
      }

      std::unique_ptr<IShader> Driver::make_shader_repr()
      {
        return std::make_unique<GL_Shader>(*this);
      }
      void Driver::make_shaders(std::size_t num, std::unique_ptr<IShader>* ss)
      {
        for(std::size_t i = 0; i < num; ++i)
        {
          ss[i] = make_shader_repr();
        }
      }
      void Driver::use_shader(IShader& s, bool force)
      {
        if(&s == cur_shader_ && !force) return;

        auto shader_ptr = CAST_PTR<GL_Shader*>(&s);
        REDC_ASSERT_MSG(shader_ptr, "Shader not made with this driver;"
                        " something is very wrong.");

        shader_ptr->use();
        cur_shader_ = shader_ptr;
      }
      IShader* Driver::active_shader() const
      {
        return cur_shader_;
      }

      std::unique_ptr<IMesh> Driver::make_mesh_repr()
      {
        // Make it and forget about it, we'll just use a dynamic cast for
        // simplicity.
        return std::make_unique<GL_Mesh>(*this);
      }
      void Driver::make_meshes(std::size_t num, std::unique_ptr<IMesh>* meshes)
      {
        for(std::size_t i = 0; i < num; ++i)
        {
          meshes[i] = make_mesh_repr();
        }
      }

      void Driver::bind_mesh(IMesh& mesh, bool force)
      {
        if(&mesh == cur_mesh_ && !force) return;

        auto gl_mesh = CAST_PTR<GL_Mesh*>(&mesh);
        REDC_ASSERT_MSG(gl_mesh, "Mesh not made with this driver; something is"
                        " very wrong.");

        gl_mesh->bind();
        cur_mesh_ = gl_mesh;
      }
      void Driver::unbind_mesh()
      {
        glBindVertexArray(0);
        cur_mesh_ = nullptr;
      }

      std::unique_ptr<ITexture> Driver::make_texture_repr()
      {
        // build n' forget.
        return std::make_unique<GL_Texture>(*this);
      }
      void Driver::make_textures(std::size_t num, std::unique_ptr<ITexture>* texs)
      {
        for(std::size_t i = 0; i < num; ++i)
        {
          texs[i] = make_texture_repr();
        }
      }
      void Driver::bind_texture(ITexture& tex, Texture_Target target)
      {
        GLenum gl_target = to_gl_texture_target(target);
        bind_texture(tex, gl_target);
      }
      void Driver::bind_texture(ITexture& tex, GLenum target)
      {
        if(&tex == cur_texture_ && target == cur_texture_target_) return;

        auto gl_tex = CAST_PTR<GL_Texture*>(&tex);
        REDC_ASSERT_MSG(gl_tex, "Texture not made with this driver; something"
                        " is very wrong.");
        gl_tex->bind(target);

        cur_texture_ = &tex;
        cur_texture_target_ = target;
      }
      void Driver::active_texture(Texture_Slot loc)
      {
        glActiveTexture(GL_TEXTURE0 + loc);

        // Clear the active texture and target because they no longer apply
        cur_texture_target_ = 0;
        cur_texture_ = nullptr;
      }

      std::size_t Driver::num_texture_slots()
      {
        int units;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &units);
        return static_cast<std::size_t>(units);
      }

      std::unique_ptr<IFramebuffer> Driver::make_framebuffer_repr()
      {
        return std::make_unique<GL_Framebuffer>(*this);
      }
      void Driver::bind_framebuffer(IFramebuffer& buf, Fbo_Binding target)
      {
        GLenum gl_target = to_gl_fbo_binding(target);
        bind_framebuffer(buf, gl_target);
      }

      GL_Framebuffer* to_gl_fbo(IFramebuffer& buf)
      {
        GL_Framebuffer* gl_fbo = CAST_PTR<GL_Framebuffer*>(&buf);
        REDC_ASSERT_MSG(gl_fbo, "Framebuffer not made with this driver;"
                        " something is very wrong.");
        return gl_fbo;
      }
      void Driver::bind_framebuffer(IFramebuffer& buf, GLenum target)
      {
        if(bound_framebuffer_ == &buf && cur_fbo_target_ == target) return;

        GL_Framebuffer* gl_fbo = to_gl_fbo(buf);
        gl_fbo->bind(target);

        bound_framebuffer_ = &buf;
        cur_fbo_target_ = target;
      }
      void Driver::use_framebuffer_draw_buffers(std::size_t num,
                                                Draw_Buffer* bufs)
      {
        // Set draw buffers
        std::vector<GLenum> draw_buffers(num);
        for(std::size_t i = 0; i < num; ++i)
        {
          draw_buffers[i] = to_gl_draw_buffer(bufs[i]);
        }

        glDrawBuffers(num, &draw_buffers[0]);

        is_default_draw_buffers_ = false;
      }
      void Driver::use_default_draw_buffers()
      {
        if(is_default_draw_buffers_ == true) return;

        // Unbind the current framebuffer. This is important, otherwise
        // GL_BACK_LEFT doesn't exist.
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        bound_framebuffer_ = nullptr;

        glDrawBuffer(GL_BACK_LEFT);

        is_default_draw_buffers_ = true;
      }

      std::unique_ptr<IRenderbuffer> Driver::make_renderbuffer_repr()
      {
        return std::make_unique<GL_Renderbuffer>(*this);
      }
      void Driver::bind_renderbuffer(IRenderbuffer& buf)
      {
        if(&buf == cur_renderbuffer_) return;

        GL_Renderbuffer* gl_rb = CAST_PTR<GL_Renderbuffer*>(&buf);
        REDC_ASSERT_MSG(gl_rb, "Renderbuffer not made with this driver;"
                        " something is very wrong.");
        gl_rb->bind();

        cur_renderbuffer_ = &buf;
      }

      void Driver::set_clear_color(Color const& c)
      {
        glClearColor(c.r / (float) 0xff, c.g / (float) 0xff,
                     c.b / (float) 0xff, c.a / (float) 0xff);
      }
      void Driver::set_clear_depth(float f)
      {
        glClearDepth(f);
      }

      void Driver::clear()
      {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      }
      void Driver::clear_color()
      {
        glClear(GL_COLOR_BUFFER_BIT);
      }
      void Driver::clear_depth()
      {
        glClear(GL_DEPTH_BUFFER_BIT);
      }
      void Driver::depth_test(bool enable)
      {
        if(enable) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);
      }
      void Driver::write_depth(bool enable)
      {
        glDepthMask(enable);
      }
      void Driver::blending(bool enable)
      {
        if(enable) glEnable(GL_BLEND);
        else glDisable(GL_BLEND);
      }
      void Driver::face_culling(bool enable)
      {
        if(enable) glEnable(GL_CULL_FACE);
        else glDisable(GL_CULL_FACE);
      }
      void Driver::cull_side(Cull_Side side)
      {
        switch(side)
        {
        case Cull_Side::Front:
          glCullFace(GL_FRONT);
          break;
        case Cull_Side::Back:
          glCullFace(GL_BACK);
          break;
        default:
          break;
        }
      }
      void Driver::set_blend_policy(Blend_Policy policy)
      {
        switch(policy)
        {
        case Blend_Policy::Transparency:
          glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
          glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE,
                              GL_ZERO);
          break;
        case Blend_Policy::Additive:
          glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
          glBlendFunc(GL_ONE, GL_ONE);
          break;
        }
      }
      void Driver::check_error()
      {
        if(glGetError() == GL_INVALID_OPERATION)
        {
          log_e("OpenGL: Invalid operation");
        }
      }
    }
  }
}
