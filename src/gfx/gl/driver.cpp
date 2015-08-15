/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "driver.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"

#include "common.h"

#include "../../common/color.h"
#include "../../common/log.h"

#include "../../common/debugging.h"

#ifdef GAME_DEBUGGING_ENABLED
#define CAST_PTR dynamic_cast
#else
#define CAST_PTR reinterpret_cast
#endif

namespace game
{
  namespace gfx
  {
    namespace gl
    {
      Driver::Driver(Vec<int> size) noexcept : IDriver(size)
      {
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE,
                            GL_ZERO);
      }
      Driver::~Driver() noexcept
      {
        glUseProgram(0);
      }

      std::unique_ptr<Shader> Driver::make_shader_repr() noexcept
      {
        return std::make_unique<GL_Shader>(*this);
      }
      void Driver::use_shader(Shader& s) noexcept
      {
        if(&s == cur_shader_) return;

        auto shader_ptr = CAST_PTR<GL_Shader*>(&s);
        if(!shader_ptr) return;

        shader_ptr->use();
        cur_shader_ = shader_ptr;
      }
      Shader* Driver::active_shader() const noexcept
      {
        return cur_shader_;
      }

      std::unique_ptr<Mesh> Driver::make_mesh_repr() noexcept
      {
        // Make it and forget about it, we'll just use a dynamic cast for
        // simplicity.
        return std::make_unique<GL_Mesh>(*this);
      }
      void Driver::bind_mesh(Mesh& mesh) noexcept
      {
        if(&mesh == cur_mesh_) return;

        auto gl_mesh = CAST_PTR<GL_Mesh*>(&mesh);
        if(!gl_mesh) return;

        gl_mesh->bind();
        cur_mesh_ = gl_mesh;
      }

      std::unique_ptr<Texture> Driver::make_texture_repr() noexcept
      {
        // build n' forget.
        return std::make_unique<GL_Texture>();
      }
      void Driver::bind_texture(Texture& tex, unsigned int loc) noexcept
      {
        auto gl_tex = CAST_PTR<GL_Texture*>(&tex);
        if(!gl_tex) return;
        gl_tex->bind(loc);
      }

      void Driver::clear_color_value(Color const& c) noexcept
      {
        glClearColor(c.r / (float) 0xff, c.g / (float) 0xff,
                     c.b / (float) 0xff, 1.0);
      }
      void Driver::clear_depth_value(float f) noexcept
      {
        glClearDepth(f);
      }

      void Driver::clear() noexcept
      {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      }
      void Driver::clear_color() noexcept
      {
        glClear(GL_COLOR_BUFFER_BIT);
      }
      void Driver::clear_depth() noexcept
      {
        glClear(GL_DEPTH_BUFFER_BIT);
      }
      void Driver::depth_test(bool enable) noexcept
      {
        if(enable) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);
      }
      void Driver::blending(bool enable) noexcept
      {
        if(enable) glEnable(GL_BLEND);
        else glDisable(GL_BLEND);
      }
      void Driver::face_culling(bool enable) noexcept
      {
        if(enable) glEnable(GL_CULL_FACE);
        else glDisable(GL_CULL_FACE);
      }
      float Driver::read_pixel(Framebuffer fb, Vec<int> pt) noexcept
      {
        auto f = get_gl_pixel_format(fb);

        float ret;
        glReadPixels(pt.x, window_extents().y - pt.y, 1, 1, f, GL_FLOAT, &ret);
        return ret;
      }
      void Driver::check_error() noexcept
      {
        if(glGetError() == GL_INVALID_OPERATION)
        {
          log_e("Invalid operation");

          int* ptr = nullptr;
          *ptr = 5;
        }
      }
    }
  }
}
