/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "driver.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"
#include "../../common/software_texture.h"
#include "../../common/software_mesh.h"
#include "../../common/log.h"
namespace game
{
  namespace gfx
  {
    namespace gl
    {
      Driver::Driver(Vec<int> size) noexcept : extents_(size)
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
        return std::make_unique<GL_Shader>();
      }
      void Driver::set_shader(Shader& s) noexcept
      {
        auto shader_ptr = reinterpret_cast<GL_Shader*>(&s);
        if(shader_ptr != cur_shader_)
        {
          shader_ptr->use();
          cur_shader_ = shader_ptr;
        }
      }
      Shader* Driver::active_shader() const noexcept
      {
        return cur_shader_;
      }

      std::unique_ptr<Mesh> Driver::make_mesh_repr() noexcept
      {
        // Make it and forget about it, we'll just use a dynamic cast for
        // simplicity.
        return std::make_unique<GL_Mesh>();
      }
      void Driver::render_mesh(Mesh& mesh) noexcept
      {
        auto gl_mesh = dynamic_cast<GL_Mesh*>(&mesh);
        if(!gl_mesh) return;
        gl_mesh->bind();
        gl_mesh->draw();
      }
      void Driver::render_mesh(Mesh& mesh, std::size_t start,
                               std::size_t count) noexcept
      {
        auto gl_mesh = dynamic_cast<GL_Mesh*>(&mesh);
        if(!gl_mesh) return;
        gl_mesh->bind();
        gl_mesh->draw(start, count);
      }

      std::unique_ptr<Texture> Driver::make_texture_repr() noexcept
      {
        // build n' forget.
        return std::make_unique<GL_Texture>();
      }
      void Driver::bind_texture(Texture& tex, unsigned int loc) noexcept
      {
        auto gl_tex = dynamic_cast<GL_Texture*>(&tex);
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
